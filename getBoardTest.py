

# import the module 
from ctypes import cdll 

# load the library 
lib = cdll.LoadLibrary('./libgeek.so') 

# create a Geek class 
class Geek(object): 
  
    # constructor 
    def __init__(self): 
  
        # attribute 
        self.obj = lib.Geek_new() 
  
    # define method 
    def myFunction(self): 
        lib.Geek_myFunction(self.obj) 
  
# create a Geek class object 
f = Geek() 
  
# object method calling 
f.myFunction()
