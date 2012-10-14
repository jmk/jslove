var love = {};

love.boot = function() {
    // TODO:
    //   - parse args
    //   - verify game data
    //   - trigger nogame() if needed

    print("boot()");
}

love.init = function() {
    // TODO:
    //   - init default config
    //   - load config file
    //   - call love.conf()
    //   - init requested modules
    //   - setup screen, etc.
    //   - version check

    print("init()");

    // XXX: testing only
    love.timer = __love_init_timer();
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

        // XXX for debugging only!
        print("dt:", dt);
    }
}

//
// Main entry point.
//
love.boot();
love.init();
love.run();
