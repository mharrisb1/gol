# 👽 gol - Game of life

Launcher for creating an instance of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) in the terminal.

## Usage

```sh
# defaults
gol

# short opts
gol -a 10 -w 10 -n 1000 -d 175

# long opts
gol --height 10 --width 10 --iterations 1000 --duration 175
```

## Help

```
👽 gol - Game of life

Usage: gol [OPTIONS]

  -w, --width      <INT> Width of board          [default: 50]
  -a, --height     <INT> Height of board         [default: 50]
  -n, --iterations <INT> Iterations (-1 for inf) [default: -1]
  -d, --duration   <INT> Turn duration (ms)      [default: 300]
  -h, --help             Print this message
```
