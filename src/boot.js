var love = {};

love.path = {};

// Replace any \ with /.
love.path.normalslashes = function(p) {
    return p.replace(/\\/g , "/");
}

// Makes sure there is a slash at the end of a path.
love.path.endslash = function(p) {
    if (p.charAt(p.length - 1) != "/") {
        return p + "/";
    } else {
        return p;
    }
}

// Checks whether a path is absolute or not.
love.path.abs = function(p) {
    var tmp = love.path.normalslashes(p);

    // Path is absolute if it starts with a "/".
    if (p.charAt(0) == "/") {
        return true;
    }

    // Path is absolute if it starts with a
    // letter followed by a colon.
    if (p.match(/^[a-zA-Z]:/)) {
        return true;
    }

    // Relative.
    return false;
}

// Converts any path into a full path.
love.path.getfull = function(p) {
    if (love.path.abs(p)) {
        return love.path.normalslashes(p);
    }

    var cwd = love.filesystem.getWorkingDirectory();
    cwd = love.path.normalslashes(cwd);
    cwd = love.path.endslash(cwd);

    // Construct a full path.
    var full = cwd + love.path.normalslashes(p);

    // Remove trailing /., if applicable
    if (p.substr(p.length-2, 2) == "/.") {
        p = p.substr(0, p.length-2);
    }

    return p;
}

// Returns the leaf of a full path.
love.path.leaf = function(p) {
    var elems = p.split("/");
    return elems[elems.length-1];
}

love.createhandlers = function() {
    love.handlers = {
        keypressed: function(b, u) {
            if (love.keypressed) { love.keypressed(b, u); }
        },
        keyreleased: function(b) {
            if (love.keyreleased) { love.keyreleased(b); }
        },
        mousepressed: function (x,y,b) {
            if (love.mousepressed) { love.mousepressed(x,y,b); }
        },
        mousereleased: function (x,y,b) {
            if (love.mousereleased) { love.mousereleased(x,y,b) }
        },
        joystickpressed: function (j,b) {
            if (love.joystickpressed) { love.joystickpressed(j,b) }
        },
        joystickreleased: function (j,b) {
            if (love.joystickreleased) { love.joystickreleased(j,b) }
        },
        focus: function (f) {
            if (love.focus) { love.focus(f) }
        },
        quit: function () {
            return;
        },
    };
}

    // TODO: Support for fused games.
var is_fused_game = false;
var no_game_code = false;

love.boot = function() {
    // TODO:
    //   - parse args
    //   - verify game data
    //   - trigger nogame() if needed

    print("args:", __argv);

    print("boot()");

    // This is absolutely needed.
    love.filesystem = __init_filesystem();
    love.filesystem.init(__argv[0]);

//	local can_has_game = pcall(love.filesystem.setSource, arg0)
    var can_has_game = false;

    if (!can_has_game && __argv[1]) {
        var full_source = love.path.getfull(__argv[1]);
        var leaf = love.path.leaf(full_source);
        love.filesystem.setIdentity(leaf);
        can_has_game = love.filesystem.setSource(full_source);
    }

    print("can_has_game?", can_has_game);

    if (can_has_game && !(love.filesystem.exists("main.js")
                          || love.filesystem.exists("conf.js")))
    {
        no_game_code = true;
    }

    print("no_game_code?", no_game_code);

    if (!can_has_game) {
        love.nogame();
    }
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
        love.conf(c);
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
    love.event      = __init_event();
    love.graphics   = __init_graphics();
    love.image      = __init_image();
    love.keyboard   = __init_keyboard();
    love.timer      = __init_timer();

    if (love.event) {
        love.createhandlers();
    }

    // Setup screen here.
    if (c.screen && c.modules.graphics) {
        if (love.graphics.checkMode(c.screen.width,
                                    c.screen.height,
                                    c.screen.fullscreen)
                || (c.screen.width == 0 && c.screen.height == 0)) {
            love.graphics.setMode(c.screen.width,
                                  c.screen.height,
                                  c.screen.fullscreen,
                                  c.screen.vsync,
                                  c.screen.fsaa);
        }

        love.graphics.setCaption(c.title);
    }

    // Our first timestep, because screen creation can take some time
    if (love.timer) {
        love.timer.step();
    }

    if (love.filesystem) {
        love.filesystem.setRelease(c.release && is_fused_game);
        if (c.identity) {
            love.filesystem.setIdentity(c.identity);
        }
        if (love.filesystem.exists("main.js")) {
            require("main");
        }
    }

    if (no_game_code) {
        error("No code to run\n" +
              "Your game might be packaged incorrectly\n" +
              "Make sure main.lua is at the top level of the zip");
    }

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
        var argv = []; // XXX
        love.load(argv);
    }

    var dt = 0.0;

    // Main loop time.
    while (true) {
        // Process events.
        if (love.event) {
            love.event.pump();

            var result = love.event.poll();

            if (result) {
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
                var handler = love.handlers[type];
                if (handler) {
                    handler.apply(this, args);
                }
            }
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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//
// XXX: copied from nogame.js, in the absence of require()
//
love.nogame = function() {
    var pig_png =
        "iVBORw0KGgoAAAANSUhEUgAAAQAAAAEACAYAAABccqhmAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJ" +
        "bWFnZVJlYWR5ccllPAAAAyBpVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADw/eHBhY2tldCBiZWdp" +
        "bj0i77u/IiBpZD0iVzVNME1wQ2VoaUh6cmVTek5UY3prYzlkIj8+IDx4OnhtcG1ldGEgeG1sbnM6" +
        "eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IkFkb2JlIFhNUCBDb3JlIDUuMC1jMDYwIDYxLjEz" +
        "NDc3NywgMjAxMC8wMi8xMi0xNzozMjowMCAgICAgICAgIj4gPHJkZjpSREYgeG1sbnM6cmRmPSJo" +
        "dHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4gPHJkZjpEZXNjcmlw" +
        "dGlvbiByZGY6YWJvdXQ9IiIgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAv" +
        "IiB4bWxuczp4bXBNTT0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL21tLyIgeG1sbnM6c3RS" +
        "ZWY9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlwZS9SZXNvdXJjZVJlZiMiIHhtcDpD" +
        "cmVhdG9yVG9vbD0iQWRvYmUgUGhvdG9zaG9wIENTNSBXaW5kb3dzIiB4bXBNTTpJbnN0YW5jZUlE" +
        "PSJ4bXAuaWlkOjdCQTYxREYxOUQ5RDExRTBBNjY4RTc1NEVEQTU1MERDIiB4bXBNTTpEb2N1bWVu" +
        "dElEPSJ4bXAuZGlkOjdCQTYxREYyOUQ5RDExRTBBNjY4RTc1NEVEQTU1MERDIj4gPHhtcE1NOkRl" +
        "cml2ZWRGcm9tIHN0UmVmOmluc3RhbmNlSUQ9InhtcC5paWQ6N0JBNjFERUY5RDlEMTFFMEE2NjhF" +
        "NzU0RURBNTUwREMiIHN0UmVmOmRvY3VtZW50SUQ9InhtcC5kaWQ6N0JBNjFERjA5RDlEMTFFMEE2" +
        "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" +
        "4xkvbY9d6gAAAABJRU5ErkJggg==";

    var heart_png =
        "iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJ" +
        "bWFnZVJlYWR5ccllPAAAAyBpVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADw/eHBhY2tldCBiZWdp" +
        "bj0i77u/IiBpZD0iVzVNME1wQ2VoaUh6cmVTek5UY3prYzlkIj8+IDx4OnhtcG1ldGEgeG1sbnM6" +
        "eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IkFkb2JlIFhNUCBDb3JlIDUuMC1jMDYwIDYxLjEz" +
        "NDc3NywgMjAxMC8wMi8xMi0xNzozMjowMCAgICAgICAgIj4gPHJkZjpSREYgeG1sbnM6cmRmPSJo" +
        "dHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4gPHJkZjpEZXNjcmlw" +
        "dGlvbiByZGY6YWJvdXQ9IiIgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAv" +
        "IiB4bWxuczp4bXBNTT0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL21tLyIgeG1sbnM6c3RS" +
        "ZWY9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlwZS9SZXNvdXJjZVJlZiMiIHhtcDpD" +
        "cmVhdG9yVG9vbD0iQWRvYmUgUGhvdG9zaG9wIENTNSBXaW5kb3dzIiB4bXBNTTpJbnN0YW5jZUlE" +
        "PSJ4bXAuaWlkOjk3NUYxN0NGOUQ5RDExRTA5MTgyQjc4MkNEQkY5RkI1IiB4bXBNTTpEb2N1bWVu" +
        "dElEPSJ4bXAuZGlkOjk3NUYxN0QwOUQ5RDExRTA5MTgyQjc4MkNEQkY5RkI1Ij4gPHhtcE1NOkRl" +
        "cml2ZWRGcm9tIHN0UmVmOmluc3RhbmNlSUQ9InhtcC5paWQ6OTc1RjE3Q0Q5RDlEMTFFMDkxODJC" +
        "NzgyQ0RCRjlGQjUiIHN0UmVmOmRvY3VtZW50SUQ9InhtcC5kaWQ6OTc1RjE3Q0U5RDlEMTFFMDkx" +
        "ODJCNzgyQ0RCRjlGQjUiLz4gPC9yZGY6RGVzY3JpcHRpb24+IDwvcmRmOlJERj4gPC94OnhtcG1l" +
        "dGE+IDw/eHBhY2tldCBlbmQ9InIiPz7MtgYTAAACd0lEQVR42uyajW2CUBDHH8YBWKBJR8AJChvY" +
        "AZriBNYJrBOIE9imA6gTyAZlg9p0ATaw99IzfSEgvI9DlLvk5UUUuPu9/71PvePxKPpsA9FzYwAM" +
        "gAEwAAbAABgAA2AADIABMAAG0EcbmtzkeV7p9Z+nVQhViB8PUNK7j+nBxkF4ZgzVHMp9ydeZfAeU" +
        "d3hPVnZ/3X6HZ7IhUgQATgZQraEEJT+XDi7AwVQzcB+qjQK0zuTzJ0Xg5ACw1aWjfs1tCTg30wh+" +
        "XwH0nOVQIlUNpACw5fcNglclKx3MCYJXIYxOSqiLb2CRm75m8AKD+kJwVba2CF6gP+s2RoGlZvCq" +
        "g/syCHDtFaqxg849hGeNyQDAw2WPHFu20gZVpKbT3OEI90ypgKkDB+9RCScIG8dDfCMFDE0l5shJ" +
        "2epLgPBdMc5bGaoqowAQOPQzFnTmU6XAVViTydctA2g0Bb9lAGnfAewoAaQdDz6H/N9SAth1HMCK" +
        "eiq87XLry5UnKQBcaXUVwurcatNlJzi79ta3AoAqSDoGYKHT+tYbIjjf/nQ8NTa1DIIfFS+SbYgo" +
        "9ojSu7RNTG6yBoCpEF0YwqJqV5g8BQpLT90tMjLpt5kCJyVkqIRDi8HnptInWQsghFHdJoRDm5lK" +
        "n2wxhMOQVMIbcfDynMH6Hc76gDKDfuFF/O0eO1/qQvBRkx+21gdUqCHBlHDZL2Q49Dox8v0ApV9I" +
        "HAUf6c72LpYCJSkRYkoEbQXfyumwAYhYVB95O235TgIogJjWKCKVOW8q+04DKMwi5UnOg/g/dMlx" +
        "imvVd5AAuCXjP0kxAAbAABgAA2AADIABMAAGwAB6ab8CDACjlPepVvE3rwAAAABJRU5ErkJggg==";

    var hearts = [];
    var rings = [];

    var cx = 400;
    var cy = 300;

    var pig;
    var heart_image;

    var add_heart_ring = function(radius, number, speed) {
        var step = Math.PI * 2 / number;
        for (var i = 0; i < number; i++) {
            var heart = {
                radius: radius,
                position: step * i,
                speed: speed,
                opacity: radius / 450
            };
            hearts.push(heart);
        }
        rings.push(radius);
    }

    var update_hearts = function(dt) {
        for (var i = 0; i < hearts.length; i++) {
            var v = hearts[i];
            v.position = v.position + v.speed * dt * 0.6;
        }
    }

    var draw_hearts = function() {
        for (var i = 0; i < hearts.length; i++) {
            var v = hearts[i];
            var x = Math.cos(v.position) * v.radius + cx;
            var y = Math.sin(v.position) * v.radius + cy;

            love.graphics.setColor(255, 255, 255, v.opacity * 255);
            love.graphics.draw(heart_image, x, y,
                               v.position + 0.4,
                               1, 1, 32, 32, 0, 0);
        }
    }

    var draw_pig = function(p) {
        love.graphics.setColor(255, 255, 255, 255);
        love.graphics.draw(p.img, p.x, p.y, 0, 1, 1, 128, 128, 0, 0);
    }

    var loadimage = function(file, name) {
        return love.graphics.newImage(
            love.image.newImageData(
                love.filesystem.newFileData(file, name, "base64")));
    }

    love.load = function() {
        pig = {
            x: cx,
            y: cy,
            img: loadimage(pig_png, "pig.png"),
        }

        heart_image = loadimage(heart_png, "heart.png");

        add_heart_ring(100, 10, 2.4);
        add_heart_ring(150, 20, 2.2);
        add_heart_ring(200, 25, 2.0);
        add_heart_ring(250, 35, 1.8);
        add_heart_ring(300, 40, 1.6);
        add_heart_ring(350, 50, 1.4);
        add_heart_ring(400, 60, 1.2);
        add_heart_ring(450, 70, 1);
    }

    love.update = function(dt) {
        update_hearts(dt);
    }

    love.draw = function() {
        love.graphics.setBackgroundColor(254, 224, 238)
        draw_hearts();
        draw_pig(pig);
    }

    love.keyreleased = function(key) {
        if (key == "escape") {
            love.event.quit();
        }
    }

    love.conf = function(t) {
        t.title = "L\u00d6VE " + love._version + " (" + love._version_codename + ")";
        t.modules.audio = false;
        t.modules.sound = false;
        t.modules.physics = false;
        t.modules.joystick = false;
    }
}

//
// Main entry point.
//
love.boot();
love.init();
love.run();
