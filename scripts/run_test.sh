rm -rf infection-test
mkdir infection-test
cp resources/*.exe infection-test
cp release/infect.exe infection-test/
cd infection-test && ./infect.exe