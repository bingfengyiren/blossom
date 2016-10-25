
#/bin/sh
cd depend/tools/

echo "setup curl"
tar -zxvf curl-7.19.0.tar.gz
cd curl-7.19.0
./configure && make && make install
cd -
rm curl-7.19.0 -rf
echo "finish setup curl"

echo "setup hiredis"
unzip hiredis-master.zip
cd hiredis-master
make && make install
cd -
rm hiredis-master -rf
echo "finish setup hiredis"

echo "setup json-c"
rm /usr/local/include/json -rf
rm /usr/local/include/json-c -rf
unzip json-c-master.zip
cd json-c-master
./autogen.sh && ./configure && make && make install
cd -
rm json-c-master -rf
echo "finish setup json-c"

echo "setup libevent"
tar -zxvf libevent-2.0.10-stable.tar.gz
cd libevent-2.0.10-stable
./configure && make && make install
cd -
rm libevent-2.0.10-stable -rf
echo "finish setup libevent"

echo "setup memcached"
tar -zxvf memcached-1.4.5.tar.gz
cd memcached-1.4.5
./configure && make && make install
cd -
rm memcached-1.4.5 -rf
echo "finish setup memcached"

echo "setup libmemcached"
tar -zxvf libmemcached-0.49.tar.gz
cd libmemcached-0.49
./configure --with-memcached && make && make install
cd -
rm libmemcached-0.49 -rf
echo "finish setup libmemcached"
