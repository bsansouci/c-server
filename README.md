c-server
========
_Disclaimer: you shouldn't use this code in production, it contains some bugs even though I tried hard to avoid memory leaks_

Client/Server code to do remote function calls. I did this project in 1st year with my roommate Eric. The more commented part is server.c, as it has an interesting architecture.
We built the server so that it could receive a lot of concurrent connection without any problem. There's a listening thread waiting for a connection. Once that connection happens, it creates a thread for that specific connection whic will take care of dealing with receiving the data and pushing the query into a linked list of queries. Then there's an event thread that is looping and checking if there's any queries in the query list. If there is one, it will take it out and call the function with the name given in the query.

The idea is that the server can receive connections very quickly, but will end up dealing with them in a sequential and ordered matter. This was done to avoid too many concurrency issues.

# Run
I did this project a long time ago, so I don't remember the gcc command to compile it. I used Xcode to modify it, test it and add some comments.
All you need to do is add pthread to the linker and make sure it knows that some header files are in lib.
