--[[
    Super Mario Bros. Demo
    Author: Colton Ogden
    Original Credit: Nintendo

    Demonstrates rendering a screen of tiles.
]]

Class = require 'class'
push = require 'push'

require 'Animation'
require 'Map'
require 'Player'

-- close resolution to NES but 16:9
VIRTUAL_WIDTH = 432
VIRTUAL_HEIGHT = 243

-- actual window resolution
WINDOW_WIDTH = 1280
WINDOW_HEIGHT = 720

-- seed RNG
math.randomseed(os.time())

-- makes upscaling look pixel-y instead of blurry
love.graphics.setDefaultFilter('nearest', 'nearest')

-- an object to contain our map data
map = Map()

-- initial gameState
gameState = 'start'
lives = 3
score = 0

-- performs initialization of all objects and data needed by program
function love.load()

    -- sets up a different, better-looking retro font as our default
    smallFont = love.graphics.newFont('fonts/font.ttf', 8)
    largeFont = love.graphics.newFont('fonts/font.ttf', 24)
    love.graphics.setFont(smallFont)

    -- sets up virtual screen resolution for an authentic retro feel
    push:setupScreen(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, {
        fullscreen = false,
        resizable = true
    })

    love.window.setTitle('Super Mario 50')

    love.keyboard.keysPressed = {}
    love.keyboard.keysReleased = {}

end

-- called whenever window is resized
function love.resize(w, h)
    push:resize(w, h)
end

-- global key pressed function
function love.keyboard.wasPressed(key)
    if (love.keyboard.keysPressed[key]) then
        return true
    else
        return false
    end
end

-- global key released function
function love.keyboard.wasReleased(key)
    if (love.keyboard.keysReleased[key]) then
        return true
    else
        return false
    end
end

-- called whenever a key is pressed
function love.keypressed(key)
    if key == 'escape' then
        love.event.quit()
    elseif key == 'enter' or key == 'return' then
        if gameState == 'start' then
            gameState = 'play'
        elseif gameState == 'continue' then
            -- here must keep the map but reset the char
            map.player.x = map.tileWidth * 10 + map.camX
            map.player.y = map.tileHeight * ((map.mapHeight - 2) / 2) - map.player.height
            gameState = 'play'
        elseif gameState == 'play' then
            -- pause or do nothing
        elseif gameState == 'gameover' or gameState == 'win' then
            -- here must reset all variables
            map.music:setLooping(false)
            map.music:stop()
            score = 0
            lives = 3
            map.camX = 0
            map.player.direction = 'right'
            map.player.x = map.tileWidth * 10
            map.player.y = map.tileHeight * ((map.mapHeight - 2) / 2) - map.player.height
            map:generateWorld()
            map.music:setLooping(true)
            map.music:play()
            gameState = 'start'
        end
    end

    love.keyboard.keysPressed[key] = true
end

-- called whenever a key is released
function love.keyreleased(key)
    love.keyboard.keysReleased[key] = true
end

-- called every frame, with dt passed in as delta in time since last frame
function love.update(dt)
    if gameState == 'play' then
        map:update(dt)
    end

    -- reset all keys pressed and released this frame
    love.keyboard.keysPressed = {}
    love.keyboard.keysReleased = {}
end

-- called each frame, used to render to the screen
function love.draw()
    -- begin virtual resolution drawing
    push:apply('start')

    -- clear screen using Mario background blue
    love.graphics.clear(108/255, 140/255, 255/255, 255/255)

    -- renders our map object onto the screen
    love.graphics.translate(math.floor(-map.camX + 0.5), math.floor(-map.camY + 0.5))
    map:render()

    -- UI Messages depending on gameState
    if gameState == 'start' then
        -- UI message Start Menu game
        love.graphics.setFont(largeFont)
        love.graphics.printf('Super Alien Boss', 0, 50, VIRTUAL_WIDTH, 'center')
        love.graphics.setFont(smallFont)
        love.graphics.printf('Press Enter to begin!', 0, 80, VIRTUAL_WIDTH, 'center')
    elseif gameState == 'play' then
        love.graphics.setFont(smallFont)
        love.graphics.printf('Lives: ' .. tostring(lives), map.camX + 5, 5, VIRTUAL_WIDTH, 'left')
        love.graphics.printf('Score: ' .. tostring(score), map.camX + 5, 15, VIRTUAL_WIDTH, 'left')
    elseif gameState == 'continue' then
        -- UI message Continue
        love.graphics.setFont(largeFont)
        love.graphics.printf('You are dead...', map.camX, 50, VIRTUAL_WIDTH, 'center')
        love.graphics.printf(tostring(lives) .. " lives left!", map.camX, 80, VIRTUAL_WIDTH, 'center')
        love.graphics.setFont(smallFont)
        love.graphics.printf('Press Enter to continue!', map.camX, 110, VIRTUAL_WIDTH, 'center')
    elseif gameState == 'gameover' then
        -- UI message GameOver
        love.graphics.setFont(largeFont)
        love.graphics.printf('Game Over!', map.camX, 50, VIRTUAL_WIDTH, 'center')
        love.graphics.printf('Score: ' .. tostring(score), map.camX, 80, VIRTUAL_WIDTH, 'center')
        love.graphics.setFont(smallFont)
        love.graphics.printf('Press Enter to restart!', map.camX, 110, VIRTUAL_WIDTH, 'center')
    elseif gameState == 'win' then
        -- UI message Win
        love.graphics.setFont(largeFont)
        love.graphics.printf('Congratulations!', map.camX, 50, VIRTUAL_WIDTH, 'center')
        love.graphics.printf('Score: ' .. tostring(score), map.camX, 80, VIRTUAL_WIDTH, 'center')
        love.graphics.setFont(smallFont)
        love.graphics.printf('Press Enter to restart!', map.camX, 110, VIRTUAL_WIDTH, 'center')
    end

    -- end virtual resolution
    push:apply('end')
end