sensitvity = 10
speed = 10

function createObject(obj)
end

function init()
	--Create stuff for GUI
	fpsCounter = GUI.createText()
	fpsCounter:setCharSize(26)
	fpsCounter:setString("FPS: ".. 0)
	--Create stuff for scene
	cam = camera.createCam()
	cam:setPos(0, 0, 0)
	camera.setCam(cam)

	fixit = GO.loadIQM("mr_fixit.iqm")
	floor = GO.loadIQM("cube.iqm")
	shit = GO.loadIQM("monkey.iqm")

	floor:setScale(10, 0.1, 10)
	floor:setPos(0, -0.5, 0)
	shit:setPos(6, 1.1, 0)
end

time = 0
frames = 0;

function update(dt)
	time = time + dt
	frames = frames + 1
	if time > 1.0 then
		fpsCounter:setString("FPS: " .. frames)
		frames = 0
		time = 0
	end
	--Move camera with standard FPS controls
	local mousex, mousey = input.getMousePos()
	mousex = mousex - (width/2)
	mousey = mousey - (height/2)
	cam:turn(mousex*sensitvity*dt, 
		mousey*sensitvity*dt)
	input.setMousePos(width/2, height/2)

	if input.isKeyDown(keys.W) then 
		cam:move(speed*dt)
	end 
	if input.isKeyDown(keys.S) then
		cam:move(-speed*dt)
	end
	if input.isKeyDown(keys.A) then
		cam:strafe(-speed*dt)
	end
	if input.isKeyDown(keys.D) then
		cam:strafe(speed*dt)
	end
	if input.isKeyDown(keys.J) then
		network.sendPacket("forward")
	end
end
