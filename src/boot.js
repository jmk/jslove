var love = {};

love.boot = function() {
    // TODO:
    //   - parse args
    //   - verify game data
    //   - trigger nogame() if needed

    print("boot()");

    // This is absolutely needed.
//    love.filesystem = __love_init_filesystem();
}

love.init = function() {
    // TODO:
    //   - load config file
    //   - call love.conf()
    //   - init requested modules
    //   - setup screen, etc.
    //   - version check

    print("init()");

    var c = {
        title: "Untitled",
        author: "Unnamed",
        version: love._version,
        screen: {
            width: 800,
            height: 600,
            fullscreen: false,
            vsync: true,
            fsaa: 0,
        },
        modules: {
            event: true,
            keyboard: true,
            mouse: true,
            timer: true,
            joystick: true,
            image: true,
            graphics: true,
            audio: true,
            physics: true,
            sound: true,
            font: true,
            thread: true,
        },
        console: false, // Only relevant for windows.
        identity: false,
        release: false,
    };

    // If config file exists, load it and allow it to update config table.
    if (!love.conf && love.filesystem && love.filesystem.exists("conf.js")) {
        require("conf");
    }

    // Yes, conf.lua might not exist, but there are other ways of making
    // love.conf appear, so we should check for it anyway.
    if (love.conf) {
        love.conf();
    }

    if (c.release) {
        love._release = {
            title: (c.title != "Untitled") ? c.title : undefined,
            author: (c.author != "Unnamed") ? c.author : undefined,
            url: c.url,
        }
    }

//    if (love.arg.options.console.set) {
//        c.console = true;
//    }

    // Load requested modules.
    // XXX: temp
    love.timer = __love_init_timer();

    if (love.event) {
        love.createhandlers();
    }

//	-- Setup screen here.
//	if c.screen and c.modules.graphics then
//		if love.graphics.checkMode(c.screen.width, c.screen.height, c.screen.fullscreen) or (c.screen.width == 0 and c.screen.height == 0) then
//			assert(love.graphics.setMode(c.screen.width, c.screen.height, c.screen.fullscreen, c.screen.vsync, c.screen.fsaa), "Could not set screen mode")
//		else
//			error("Could not set screen mode")
//		end
//		love.graphics.setCaption(c.title)
//	end
//
//	-- Our first timestep, because screen creation can take some time
//	if love.timer then
//		love.timer.step()
//	end
//
//	if love.filesystem then
//		love.filesystem.setRelease(c.release and is_fused_game)
//		if c.identity then love.filesystem.setIdentity(c.identity) end
//		if love.filesystem.exists("main.lua") then require("main") end
//	end
//
//	if no_game_code then
//		error("No code to run\nYour game might be packaged incorrectly\nMake sure main.lua is at the top level of the zip")
//	end
//
//	-- Console hack
//	if c.console and love._openConsole then
//		love._openConsole()
//	end
//
//	-- Check the version
//	local compat = false
//	c.version = tostring(c.version)
//	for i, v in ipairs(love._version_compat) do
//		if c.version == v then
//			compat = true
//			break
//		end
//	end
//	if not compat then
//		local major, minor, revision = c.version:match("^(%d+)%.(%d+)%.(%d+)$")
//		if (not major or not minor or not revision) or (major ~= love._version_major and minor ~= love._version_minor) then
//			local msg = "This game was made for a version that is probably incompatible.\n"..
//				"The game might still work, but it is not guaranteed.\n" ..
//				"Furthermore, this means one should not judge this game or the engine if not."
//			print(msg)
//			if love.graphics and love.timer and love.event then
//				love.event.pump()
//				love.graphics.setBackgroundColor(89, 157, 220)
//				love.graphics.clear()
//				love.graphics.print(msg, 70, 70)
//				love.graphics.present()
//				love.graphics.setBackgroundColor(0, 0, 0)
//				love.timer.sleep(3)
//			end
//		end
//	end

    // XXX: testing only; delete
    print("version: ",  love._version);
    print("major: ",    love._major);
    print("minor: ",    love._minor);
    print("revision: ", love._revision);
    print("codename: ", love._codename);
    print("os: ",       love._os);
}

love.run = function() {
    // TODO:
    //   - pass along correct argv

    if (love.load) {
        var argv = [];
        love.load(argv);
    }

    var dt = 0.0;

    // Main loop time.
    while (true) {
        // Process events.
        if (love.event) {
            love.event.pump();

            var result = love.event.poll();
            var type = result[0];

            if (type == "quit") {
                if (!love.quit || !love.quit()) {
                    if (love.audio) {
                        love.audio.stop();
                    }
                    return;
                }
            }

            var args = result.slice(1);
            love.handlers[type].apply(this, args);
        }

        // Update dt, as we'll be passing it to update.
        if (love.timer) {
            love.timer.step();
            dt = love.timer.getDelta();
        }

        // Call update and draw.
        if (love.update) {
            // Note: This will pass 0 for dt if love.timer is disabled.
            love.update(dt);
        }

        if (love.graphics) {
            love.graphics.clear();
            if (love.draw) {
                love.draw();
            }
        }

        if (love.timer) {
            love.timer.sleep(0.001);
        }

        if (love.graphics) {
            love.graphics.present();
        }
    }
}

//
// Main entry point.
//
love.boot();
love.init();
love.run();
