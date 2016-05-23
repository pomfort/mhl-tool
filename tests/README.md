## Automated MHL Tool Tests

To run some of the MHL tool test cases you need to install lettuce (http://lettuce.it/)

#### Running the tests on OS X

Build the mhl tool with Release configuration using the XCode project.

From within the `tests` folder run the following commands:


```
#running python unit tests
python -m unittest discover -s mhl_unittests -t . -v

#Run lettuce tests, use UTF-8, else the file encoding test cases will fail
env LANG="en_US.UTF-8" LANGUAGE="en_US.UTF-8" LC_ALL="en_US.UTF-8" /usr/local/bin/lettuce --no-color --failfast 
```