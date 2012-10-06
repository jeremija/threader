THREADER
--------

An utility for launching other applications in more processes. For example, run you can run a lame converter in 4 threads to convert all files in the specific directory so that you can minimize the time neccessary for conversion. 

HOW TO INSTALL
--------------

Run `https://github.com/jeremija/threader` and then `cd threader`. After that you need to run this commands:

    make
    make install-config
    sudo make install

The `make install-config` will copy and rename the `config` folder to `~/.threader`. This is threader's config folder. If you have already installed the application before and you do not wish that folder to be replaced, feel free to avoid running that command, but check for the differences in config file. 

The `sudo make install` will only copy the `threader` binary to `/usr/local/bin/` folder.

To uninstall run `sudo make uninstall`, this will only delete the `/usr/local/bin/threader` file. You can delete the `~/.threader` folder later if you wish.
