# Paralelized Blur Effect

```
Authors:

Nicolás Viveros - nviverosb@unal.edu.co
Ivan Herrera - biherrerap@unal.edu.co

30 April 2019.
```

Original project can be found [here][github] 

[github]: https://github.com/viversba/Parallel

This folder contains all the necessary files to run a simple blur effect algorithm on a given `.png` file. 

##Dependencies

* This project uses the `png++` library, which can be found [here][png] 

[png]: https://www.nongnu.org/pngpp/

## Compilation
 In order to compile the file, the following flags have to be used:
 
 * ``` `libpng-config --ldflags` ```
 * ``` -pthread ```

 So the whole compilation line should look something like this:
 
 ``` g++ -o main main.o `libpng-config --ldflags` -pthread ```
 
## Licensing
Code released under the [GNU General Public License][license].
[license]: https://opensource.org/licenses/GPL-3.0  
