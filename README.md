# FlyingFalcon

a network framework base on asio
 
falcon flying above in the sky freely.


Asio(non-Boost edition, http://think-async.com/)

and Asio is hosted on GitHub.
https://github.com/chriskohlhoff/asio/

git submodule add https://github.com/chriskohlhoff/asio.git  asio

useage:

git clone https://github.com/machh/FlyingFalcon

cd FlyingFalcon

git submodule update --init --recursive



note:
   “remote: Support for password authentication was removed on August 13, 2021.“


linux:
    ssh-keygen -t rsa
    cat ~/.ssh/id_rsa.pub   

windows    
    ssh-keygen -t rsa -b 4096 -C "email"
    cd C:\Users\Administrator\.ssh
    copy id_rsa.pub to GitHub SSH keys
    
    git push origin master



