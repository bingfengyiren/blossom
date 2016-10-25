#!/bin/bash
# remove a so file
cp ../conf/work_config.ini.stop ../conf/work_config.ini
python2.6 binserverclient.py 10.77.96.33 72169 'update_work'

# add a so file
cp ../conf/work_config.ini.start ../conf/work_config.ini
python2.6 binserverclient.py 10.77.96.33 72169 'update_work'
