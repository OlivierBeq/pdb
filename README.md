# pdb

A simple utility to download files from the Protein Data Bank

Works currently only under Unix

# Installation
```
sudo apt install build-essential cmake libcurl4-openssl-dev wget gzip

cd pdbget
mkdir build && cd build
cmake .. && make && mv src/pdbget . && rm -r src
```
