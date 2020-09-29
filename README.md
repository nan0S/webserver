# webserver
Simple WWW server that accepts HTTP requests and sends back HTTP responses, written in C. The server uses **TCP**.

##### Usage
In order to run the server, type the following command: ```./webserver PORT CATALOG```, where ```PORT``` is the port that server listens on and ```CATALOG``` is the directory with webpages (web resources, files, etc.), eg. ```./webserver 7777 webpages/```.

Screenshots
---
#### Example webpage
Webpage shown below is created after running the server with the command: ```./webserver 8888 webpages/```. Then, address localhost:8888 has been typed into the browser (```Firefox``` on the screenshot):
![](https://i.imgur.com/diDLZle.png)