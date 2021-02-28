WINDOW_WIDTH = 1280
WINDOW_HEIGHT = 720

VIRTUAL_WIDTH = 432
VIRTUAL_HEIGHT = 243

PADDLE_SPEED = 200

Class = require 'class'
push = require 'push'

require 'Paddle'
require 'Ball'

function love.load()

    math.randomseed(os.time())

    love.window.setTitle('Pong')

    love.graphics.setDefaultFilter('nearest', 'nearest')

    push:setupScreen(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, {
        fullscreen = false,
        vsync = true,
        resizable = true
    })

    -- initialize the fonts
    smallFont = love.graphics.newFont('font.ttf', 8)
    scoreFont = love.graphics.newFont('font.ttf', 32)
    victoryFont = love.graphics.newFont('font.ttf', 24)
    love.graphics.setFont(smallFont)

    -- initialize a table of sounds
    sounds = {
        ['paddle_hit'] = love.audio.newSource('sounds/paddle_hit.wav', 'static'),
        ['point_scored'] = love.audio.newSource('sounds/score.wav', 'static'),
        ['wall_hit'] = love.audio.newSource('sounds/wall_hit.wav', 'static')
    }

    player1Score = 0
    player2Score = 0
    servingPlayer = math.random(2) == 1 and 1 or 2
    winningPlayer = 0

    paddle1 = Paddle(5, 20, 5, 20)
    paddle2 = Paddle(VIRTUAL_WIDTH - 10, VIRTUAL_HEIGHT - 40, 5, 20)
    ball = Ball(VIRTUAL_WIDTH / 2 - 2, VIRTUAL_HEIGHT / 2 - 2, 4, 4)

    if servingPlayer == 1 then
        ball.dx = 100
    else
        ball.dx = -100
    end


    gameState = 'start'

end

function love.resize(w, h)
    push:resize(w, h)
end

function love.update(dt)

    if gameState == 'play' then

        -- if we reach the side of the screen, update score and prepare next serving player
        if ball.x <= 0 then
            servingPlayer = 1
            player2Score = player2Score + 1
            ball:reset()
            ball.dx = 100
            sounds['point_scored']:play()
            
            if player2Score >= 3 then
                gameState = 'victory'
                winningPlayer = 2
            else
                gameState = 'serve'
            end
        end

        if ball.x >= VIRTUAL_WIDTH - 4 then
            servingPlayer = 2
            player1Score = player1Score + 1
            ball:reset()
            ball.dx = -100
            sounds['point_scored']:play()

            if player1Score >= 3 then
                gameState = 'victory'
                winningPlayer = 1
            else
                gameState = 'serve'
            end
        end

        if ball:collides(paddle1) then
            -- deflect ball to the right slightly increasing speed
            ball.dx = -ball.dx * 1.03
            ball.x = paddle1.x + 5
            sounds['paddle_hit']:play()

            if ball.y <= 0 then
                -- keep velocity going in the same direction but randomize it
                ball.dy = -math.random(10, 150)
            else
                ball.dy = math.random(10, 150)
            end
        end

        if ball:collides(paddle2) then
            -- deflect ball to the left
            ball.dx = -ball.dx * 1.03
            ball.x = paddle2.x - 4
            sounds['paddle_hit']:play()

            if ball.y <= 0 then
                -- keep velocity going in the same direction but randomize it
                ball.dy = -math.random(10, 150)
            else
                ball.dy = math.random(10, 150)
            end
        end

        -- detect upper and lower screen boundary collision and reverse if collided
        if ball.y <= 0 then
            ball.y = 0
            ball.dy = -ball.dy
            sounds['wall_hit']:play()
        end

        -- -4 to account for the ball's size
        if ball.y >= VIRTUAL_HEIGHT - 4 then
            ball.y = VIRTUAL_HEIGHT - 4
            ball.dy = -ball.dy
            sounds['wall_hit']:play()
        end
    end

    -- player 1 movement
    if love.keyboard.isDown('w') then
        paddle1.dy = -PADDLE_SPEED
    elseif love.keyboard.isDown('s') then
        paddle1.dy = PADDLE_SPEED
    else
        paddle1.dy = 0
    end

    -- player 2 movement
    if love.keyboard.isDown('up') then
        paddle2.dy = -PADDLE_SPEED
    elseif love.keyboard.isDown('down') then
        paddle2.dy = PADDLE_SPEED
    else
        paddle2.dy = 0
    end

    -- update our ball based on its DX and DY only if we're in play state
    -- scale the velocity by dt so movement is framerate independant
    if gameState == 'play' then
        ball:update(dt)
    end

    paddle1:update(dt)
    paddle2:update(dt)

end

function love.keypressed(key)

    if key == 'escape' then
        love.event.quit()
    elseif key == 'enter' or key == 'return' then
        if gameState == 'start' then
            gameState = 'serve'
        elseif gameState == 'serve' then
            gameState = 'play'
        elseif gameState == 'victory' then
            gameState = 'start'
            player1Score = 0
            player2Score = 0
        end
    end

end

function love.draw()

    push:apply('start')

    love.graphics.clear(40/255, 45/255, 52/255, 255/255)

    -- render the ball
    ball:render()

    -- render first paddle
    paddle1:render()
    paddle2:render()
    
    -- main title
    love.graphics.setFont(smallFont)

    if gameState == 'start' then
        love.graphics.printf("Welcome to Pong", 0, 20, VIRTUAL_WIDTH, 'center')
        love.graphics.printf("Press Enter to Play!", 0, 32, VIRTUAL_WIDTH, 'center')
    elseif gameState == 'serve' then
        love.graphics.printf("Player " .. tostring(servingPlayer) .. "'s turn!", 0, 20, VIRTUAL_WIDTH, 'center')
        love.graphics.printf("Press Enter to Serve!", 0, 32, VIRTUAL_WIDTH, 'center')
    elseif gameState == 'victory' then
        -- draw a victory message
        love.graphics.setFont(victoryFont)
        love.graphics.printf("Player " .. tostring(servingPlayer) .. " wins", 0, 20, VIRTUAL_WIDTH, 'center')
        love.graphics.printf("Press Enter to Serve!", 0, 42, VIRTUAL_WIDTH, 'center')
        love.graphics.setFont(smallFont)
    end

    -- score
    love.graphics.setFont(scoreFont)
    love.graphics.print(player1Score, VIRTUAL_WIDTH / 2 - 50, VIRTUAL_HEIGHT / 3)
    love.graphics.print(player2Score, VIRTUAL_WIDTH / 2 + 30, VIRTUAL_HEIGHT / 3)

    -- display the FPS
    displayFPS()

    push:apply('end')
 
end

function displayFPS()
    love.graphics.setColor(0, 1, 0, 1)
    love.graphics.setFont(smallFont)
    love.graphics.print('FPS: ' .. tostring(love.timer.getFPS()), 40, 20)
    love.graphics.setColor(1, 1, 1, 1)
end