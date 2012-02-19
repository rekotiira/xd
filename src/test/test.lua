xd.scheduler = function()
	return {
		threads = {},
		start = function(self, func)
			local thread = coroutine.create(func)
			local status, cond = coroutine.resume(thread)
			if status and coroutine.status(thread) == "suspended" then
				table.insert(self.threads, {thread, cond})
			end
		end,
		run = function(self)
			-- check for suspended threads
			local i = 1
			while self.threads[i] do
				local thread = self.threads[i]
				if thread[2]() then
					local status, cond = coroutine.resume(thread[1]);
					if status and coroutine.status(thread[1]) == "suspended" then
						thread[2] = cond
						i = i + 1
					else
						table.remove(self.threads, i)
					end
				else
					i = i + 1
				end
			end
		end
	}
end

function delayed_fire()
	print("preparing to fire")
	wait(2)
	print("fire!")
end

function wait(secs)
	local begin = os.time()
	coroutine.yield(function() return (os.difftime(os.time(), begin) >= secs) end)
end

win = xd.window("test window", 640, 480)
win:bind_key(xd.KEY_SPACE, "fire")
win:bind_key(xd.MOUSE_LEFT, "fire")
win:bind_key(xd.KEY_ESC, "quit")

scheduler = xd.scheduler()

while true do
	win:update()

	if win:closed() or win:triggered("quit") then
		break
	end

	if win:triggered("fire") then
		scheduler:start(delayed_fire)
	end

	-- update scheduler
	scheduler:run()

	win:clear()
	win:swap()
end