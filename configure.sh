#!/usr/bin/env bash
# Generate the data file.
truncate -s 2g /tmp/2GB.file
# TODO: Restrict memory usage of a program https://thirld.com/blog/2012/02/09/things-to-remember-when-using-ulimit/