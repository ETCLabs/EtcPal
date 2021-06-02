# Install dependencies
apt-get update
apt-get install -y --no-install-recommends wget ca-certificates libclang1-9 libclang-cpp9

# Install Doxygen
wget https://doxygen.nl/files/doxygen-1.9.1.linux.bin.tar.gz
tar xzvf doxygen-1.9.1.linux.bin.tar.gz > /dev/null
cp doxygen-1.9.1/bin/doxygen /usr/bin/
