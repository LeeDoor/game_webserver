# http request API
## file API
to request static file from server, set relative path to root folder in request target like this: 
```
127.0.0.1/folder1/index.html
```
this url will return index.html file in folder1.
to get main index.html file it is not required to write file name. these two lines of request will get same response:
```
127.0.0.1/index.html
127.0.0.1/
``` 

## in-game API
empty yet