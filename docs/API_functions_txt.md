# what is it for
API_functions.txt document describes all api methods that this server provides. i found it more comfortable way to change function properties because it doesn't require project rebuilding.

# example of usage
```txt
/api/profile
GET HEAD
ApiGetProfileData
Authorization required
```

here we can see an example of function declaration in this file. 
* first line is a target line of HTTP request after ip and port. e.g. this function call will look like this for local host: `127.0.0.1:9999/api/profile`  where 9999 is a port
* second line is space-separated CAPS-written allowed methods. 
* third line shows text definition for our api function. this function should be in `src/http_handler/api_handler.cpp` file. also in function `ApiFunctionsParse` you need to define mapping from string name to real reference to function e.g. `{ "ApiGetProfileData", BIND(&ApiHandler::ApiGetProfileData) }`
* after third line you need to list unnecessary headers. here is the list of them: #TODO to supplement the documentation as the project grows. 
	- `Authorization required`: means that function requires authorization header. in HTTP request there should be header, which provides user's token. to gain it, you need to register and login.
	- `Debug method`: means that function is callable only for admins. requires admin's credentials in HTTP request body.