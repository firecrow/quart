# Quart

![quart logo](http://quart.firecrow.com/quart-logo128.png)


Quart is a block based programming language, with few symbols and explict references of lexical or direct scope


```
:x 3 :y 3; :z * x y
:out {
    write h
    write j
}

out :h 10 :j "there"

:min {
    .x 10 
    :z 0 
    if { x < z } 
    then {x} 
    else {z}
} 

min :z 5; 
min :z 17
&x 5 
&y 10
```

visit [http://quart.firecrow.com](http://quart.firecrow.com) for detaill.
