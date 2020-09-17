import os
import requests

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    query = db.execute("SELECT * FROM portfolio WHERE users_id=:id", id=session["user_id"])
    grand_total = 0.0
    for row in query:
        total = float(row["avg_price"]) * float(row["quantity"])
        grand_total += total
        row["avg_price"] = usd(row["avg_price"])
        row["total"] = usd(total)

    cash = db.execute("SELECT cash FROM users WHERE id=:id", id=session["user_id"])
    grand_total += cash[0]["cash"]
    cash = usd(cash[0]["cash"])
    grand_total = usd(grand_total)

    return render_template("index.html", query=query, cash=cash, grand_total=grand_total)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():

    if request.method == "POST":

        # Ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("must provide symbol", 403)

        # Ensure quantity was submitted
        elif not request.form.get("quantity"):
            return apology("must provide password", 403)

        #lookup price
        response = lookup(request.form.get("symbol"))

        if response == None:
            return apology("symbol does not exist", 403)

        symbol = response["symbol"]
        name = response["name"]
        price = response["price"]
        users_id = session["user_id"]

        try:
            quantity = int(request.form.get("quantity"))
        except:
            return apology("quantity must be an integer", 403)

        if not quantity > 0:
            return apology("quantity must be > 0", 403)

        cost = float(quantity) * price

        #check users cash
        query = db.execute("SELECT cash FROM users WHERE id=:id", id=users_id)
        cash = float(query[0]["cash"])
        if cash < cost:
            return apology("cash is insufficient", 403)
        cash = cash - cost

        #add the buy order in history
        db.execute("INSERT INTO history (users_id, symbol, name, quantity, price, order_type, cash) VALUES (:users_id, :symbol, :name, :quantity, :price, :order_type, :cash)",
            users_id=users_id, symbol=symbol, name=name, quantity=quantity, price=price, order_type="buy", cash=cash)

        if not db.execute("SELECT * FROM portfolio WHERE symbol=:symbol AND users_id=:id", symbol=symbol, id=users_id):
            db.execute("INSERT INTO portfolio (users_id, symbol, name, quantity, avg_price) VALUES (:users_id, :symbol, :name, :quantity, :avg_price)",
                users_id=users_id, symbol=symbol, name=name, quantity=quantity, avg_price=price)
        else:
            query = db.execute("SELECT * FROM portfolio WHERE symbol=:symbol AND users_id=:id", symbol=symbol, id=users_id)
            print(type(query[0]["quantity"]))
            print(type(quantity))
            new_quantity = int(query[0]["quantity"]) + quantity
            new_avg_price = ((query[0]["quantity"] * query[0]["avg_price"]) + cost) / new_quantity
            db.execute("UPDATE portfolio SET symbol=:symbol, quantity=:quantity, avg_price=:avg_price WHERE id=:id",
                symbol=symbol, quantity=new_quantity, avg_price=new_avg_price, id=query[0]["id"])

        query = db.execute("UPDATE users SET cash=:cash WHERE id=:id", cash=cash, id=users_id)


        flash(f'Bought {quantity} share(s) of {symbol} for a total of {usd(cost)}!')
        return redirect("/")

            #if so, compute the new average price
            #add the quantity to the users portfolio and overwrite the average price

    return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    query = db.execute("SELECT * FROM history WHERE users_id=:id", id=session["user_id"])
    for row in query:
        row["price"] = usd(row["price"])
    return render_template("history.html", query=query)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")

@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():

    if request.method == "POST":
        # Ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("must provide symbol", 403)

        response = lookup(request.form.get("symbol"))

        if response == None:
            return apology("symbol does not exist", 403)

        if response:
            return render_template("quoted.html", quote_name=response["name"], quote_symbol=response["symbol"], quote_price=usd(float(response["price"])))
        else:
            print("Request returned an error")

    return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

# User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Ensure password confirmation was submitted
        elif not request.form.get("confirm_password"):
            return apology("must confirm password", 403)

        elif request.form.get("confirm_password") != request.form.get("password"):
            return apology("password and confirmation do not match!", 403)

        # Query database for username

        if not db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username")):
            db.execute("INSERT INTO users (username, hash, cash) VALUES (:username, :hash, :cash)",
                          username=request.form.get("username"), hash=generate_password_hash(request.form.get("password")), cash=10000)

            query = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))
            session["user_id"] = query[0]["id"]

            # Redirect user to home page
            flash('Registered!')
            return redirect("/")
        else:
            return apology("username exists!")

    return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    #User wants to sell
    if request.method == "POST":

        #check if symbol is selected
        if not request.form.get("symbol"):
            return apology("must provide symbol", 403)

        #check if quantity is positive
        elif not request.form.get("quantity"):
            return apology("must provide quantity", 403)

        symbol = request.form.get("symbol")

        try:
            quantity = int(request.form.get("quantity"))
        except:
            return apology("quantity must be an integer", 403)

        if not quantity > 0:
            return apology("quantity must be > 0", 403)

        #interrogate the portfolio and check if quantity and symbol exist
        query = db.execute("SELECT * FROM portfolio WHERE users_id=:id AND symbol=:symbol", id=session["user_id"], symbol=symbol)

        new_quantity = int(query[0]["quantity"]) - quantity

        if new_quantity < 0:
            return apology("quantity of shares owned is insufficient", 403)

        #proceed to remove the quantity from portfolio and update
        elif new_quantity == 0:
            db.execute("DELETE FROM portfolio WHERE symbol=:symbol AND users_id=:id", symbol=symbol, id=session["user_id"])

        else:
            db.execute("UPDATE portfolio SET quantity=:new_quantity WHERE users_id=:id AND symbol=:symbol", new_quantity=new_quantity, id=session["user_id"], symbol=symbol)

        #lookup the sell price
        sale = lookup(symbol)

        if sale == None:
            return apology("symbol does not exist", 403)

        #obtain current cash
        cash = db.execute("SELECT cash FROM users WHERE id=:id", id=session["user_id"])
        cash = float(cash[0]["cash"])
        cash = cash + float(quantity * sale["price"])

        #record the sell operation in history
        db.execute("INSERT INTO history (users_id, symbol, name, quantity, price, order_type, cash) VALUES (:users_id, :symbol, :name, :quantity, :price, :order_type, :cash)",
            users_id=session["user_id"], symbol=symbol, name=sale["name"], quantity=(0-quantity), price=sale["price"], order_type="sell", cash=cash)

        #add the cash in users
        db.execute("UPDATE users SET cash=:cash WHERE id=:id", cash=cash, id=session["user_id"])


        flash(f'Sold {quantity} share(s) of {symbol} for a total of {usd(float(quantity * sale["price"]))}!')
        return redirect("/")

    #user with GET
    #obtain the current portfolio
    #for every unique symbol available, add to a list
    #pass the list of symbols to the webpage


    query = db.execute("SELECT * FROM portfolio WHERE users_id=:id", id=session["user_id"])
    return render_template("sell.html", query=query)


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
