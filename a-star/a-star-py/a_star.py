#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import tkinter as tk
import time
from queue import *

from distance import *
from grid import Grid

Point = tuple[int, int]
window = tk.Tk()

canvasWidth = 410
canvasHeight = 410
canvas = tk.Canvas(window, bg="white", width=canvasWidth, height=canvasHeight)
canvas.grid(row=4, column=2)

objectOption = tk.StringVar(window, 1)

options = {"Start": '1',
           "End": '2',
           "Walls": '3'}
for (text, value) in options.items():
    tk.Radiobutton(window, text=text, variable=objectOption,
                   value=value, anchor=tk.E).grid(row=value, column=3)

distanceOption = tk.StringVar(window, 1)
options = {"Euclidean": '1',
           "Manhattan": '2'}
for (text, value) in options.items():
    tk.Radiobutton(window, text=text, variable=distanceOption,
                   value=value, anchor=tk.E).grid(row=value, column=4)

findPathText = tk.StringVar()
findPathText.set("Click on the button above to find a path.")
findPathResultLabel = tk.Label(window, textvariable=findPathText)
findPathResultLabel.grid(row=3, column=2)

initial_width = 20
initial_height = 20

grid = Grid(initial_width, initial_height)

start = (0, 0)
end = (0, 0)

def draw():
    canvas.delete('all')
    grid.draw(canvas)
    minSide = min(grid.horizStep, grid.vertStep)
    create_circle((start[0]*grid.horizStep, start[1]*grid.vertStep), minSide/4, "green")
    create_circle((end[0]*grid.horizStep, end[1]*grid.vertStep), minSide/4, "red")


def placeObjectCallback(event):
    global start, end
    x = int((event.x - 5 + grid.horizStep/2) // grid.horizStep)
    y = int((event.y - 5 + grid.vertStep/2) // grid.vertStep)
    print("Clicked at", x, y)

    if objectOption.get() == '1':
        start = (x, y)
    elif objectOption.get() == '2':
        end = (x, y)
    else:
        grid.toggleWall( (x, y) )

    draw()
canvas.bind("<Button-1>", placeObjectCallback)


def a_start(grid : Grid, start : Point, end : Point, distance_func=euclidean_dist):
    q = PriorityQueue()
    q.put((0, start))

    came_from : dict[Point, Optional[Point]] = {}
    cost_from_start : dict[Point, float] = {}

    came_from[start] = None
    cost_from_start[start] = 0

    while not q.empty():
        (p, current) = q.get()

        if current == end:
            break

        neighbours = grid.getNeighbours(current)
        for n in neighbours:
            n_cost = cost_from_start[current] + distance_func(current, n)
            if n not in cost_from_start or n_cost < cost_from_start[n]:
                grid.discovered.append(n)
                cost_from_start[n] = n_cost
                priority = n_cost + distance_func(n, end)
                q.put((priority, n)) 
                came_from[n] = current

    return came_from


def reconstruct_path(came_from : dict[Point, Point],
                     end : Point) -> list[Point]:
    current : Point = end
    path : list[Point] = []
    while came_from[current] != None:
        path.append(current)
        current = came_from[current]
    path.append(current)
    path.reverse()
    return path


def create_circle(point : Point, r : int, colour : str):
    (x, y) = point
    x0 = x + 5 - r
    y0 = y + 5 - r
    x1 = x + 5 + r
    y1 = y + 5 + r
    canvas.create_oval(x0, y0, x1, y1, fill=colour)


def resizeGrid(width : int, height : int):
    grid.resize(width, height)
    draw()


def findPath():
    if start in grid.walls or end in grid.walls:
        findPathText.set("Couldn't find any path between those coordinates.")
        findPathResultLabel.config(fg="red")
        return

    pathColor = ''
    if (distanceOption.get() == '1'):
        start_time = time.time()
        came_from_list = a_start(grid, start, end, euclidean_dist)
        endtime = time.time()
        pathColor = 'blue'
    elif (distanceOption.get() == '2'):
        start_time = time.time()
        came_from_list = a_start(grid, start, end, manhattan_dist)
        endtime = time.time()
        pathColor = 'purple'
    print("A* algorithm execution time: %ss" % (endtime - start_time))

    try:
        path = reconstruct_path(came_from_list, end)
    except:
        findPathText.set("Couldn't find any path between those coordinates.")
        findPathResultLabel.config(fg="red")
        return

    grid.printPath(path, canvas, pathColor)
    findPathText.set("Path found.")
    findPathResultLabel.config(fg="green")


def generateWalls(wallCount : int):
    grid.generateRandomWalls(wallCount)
    draw()


def main():
    grid.draw(canvas)
    print()
    create_circle((start[0]*grid.horizStep, start[1]*grid.vertStep), 10, "green")
    create_circle((end[0]*grid.horizStep, end[1]*grid.vertStep), 10, "red")

    greeting = tk.Label(text="A* Visualization by mgonnav")
    greeting.grid(row=0)

    widthLabel = tk.Label(text="Columns:")
    widthLabel.grid(row=1, column=0)
    widthEntry = tk.Entry(width=10)
    widthEntry.grid(row=1, column=1)
    widthEntry.insert(0, initial_width)

    heightLabel = tk.Label(text="Rows:")
    heightLabel.grid(row=2, column=0)
    heightEntry = tk.Entry(width=10)
    heightEntry.grid(row=2, column=1)
    heightEntry.insert(0, initial_height)

    changeSizeButton = tk.Button(text="Resize",
                                 bg="blue",
                                 fg="yellow",
                                 command=lambda : resizeGrid(int(widthEntry.get()),
                                                             int(heightEntry.get())))
    changeSizeButton.grid(row=3)

    findPathButton = tk.Button(text="Find path",
                               bg="blue",
                               fg="yellow",
                               command=lambda : findPath())
    findPathButton.grid(row=2, column=2)

    generateWallsLabel = tk.Label(text="# of walls:")
    generateWallsLabel.grid(row=1, column=6)
    generateWallsEntry = tk.Entry(width=10)
    generateWallsEntry.grid(row=2, column=6)
    generateWallsEntry.insert(0, 200)
    generateWallsButton = tk.Button(text="Generate walls",
                                    bg="blue",
                                    fg="yellow",
                                    command=lambda : generateWalls(int(generateWallsEntry.get()))
                                    )
    generateWallsButton.grid(row=3, column=6)

    window.mainloop()


main()
