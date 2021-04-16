#!/usr/bin/env python
# -*- coding: utf-8 -*-
from vars import *
import random

Point = tuple[int, int]

class Grid:
    def __init__(self, width : int, height : int):
        self.width : int = width
        self.height : int = height
        self.horizStep : float = (canvasWidth - 10) / (width - 1)
        self.vertStep : float = (canvasHeight - 10) / (height - 1)
        self.grid : list[list[str]] = [['.']*width for _ in range(height)]
        self.walls : list[Point] = []
        self.discovered : list[Point] = []

    def isInside(self, point : Point) -> bool:
        (x, y) = point
        return x >= 0 and x < self.width and y >= 0 and y < self.height

    def notWall(self, point : Point) -> bool:
        return point not in self.walls

    def getNeighbours(self, point : Point) -> list[Point]:
        (x, y) = point
        neighbours = [(x - 1, y - 1), (x, y - 1), (x + 1, y - 1),
               (x - 1, y), (x + 1, y),
               (x - 1, y + 1), (x, y + 1), (x + 1, y + 1)]
        actual_neighbours = filter(self.isInside, neighbours)
        actual_neighbours = filter(self.notWall, actual_neighbours)
        return list(actual_neighbours)

    def draw(self, canvas):
        for i in range(self.width):
            canvas.create_line(self.horizStep*i + 5, 5, self.horizStep*i + 5, canvasHeight - 5)

        for i in range(self.height):
            canvas.create_line(5, self.vertStep*i + 5, canvasWidth - 5, self.vertStep*i + 5)

        minSide = min(self.horizStep, self.vertStep)
        for (x, y) in self.walls:
            canvas.create_rectangle(5 + x*self.horizStep - minSide/2 + 1,
                                    5 + y*self.vertStep - minSide/2 + 1,
                                    5 + x*self.horizStep + minSide/2 - 1,
                                    5 + y*self.vertStep + minSide/2 - 1, fill="black")

        #  for i in range(self.height):
            #  for j in range(self.width):
                #  if (i, j) in self.walls:
                    #  print('#', end=' ')
                #  else:
                    #  print('.', end=' '),
            #  print()

    def printPath(self, path : list[Point], canvas, color='blue'):
        for i in range(len(path)-1):
            (x1, y1) = path[i]
            (x2, y2) = path[i + 1]
            canvas.create_line(5 + x1*self.horizStep, 5 + y1*self.vertStep, 5 + x2*self.horizStep, 5 + y2*self.vertStep, fill=color, width=3)

        for (x, y) in path:
            self.grid[y][x] = 'x'

        #  for (x, y) in path:
            #  self.grid[y][x] = '.'
        #  for i in range(height):
            #  for j in range(width):
                #  if (i, j) in self.walls:
                    #  print('#', end=' ')
                #  elif (i, j) in  path:
                    #  print('x', end=' ')
                #  elif (i, j) in self.discovered:
                    #  print('_', end=' ')
                #  else:
                    #  print('.', end=' ')
            #  print()

    def resize(self, width : int, height : int):
        self.width = width
        self.height = height
        self.horizStep = (canvasWidth - 10) / (self.width - 1)
        self.vertStep = (canvasHeight - 10) / (self.height - 1)
        self.grid = [['.']*self.width for _ in range(self.height)]
        print("New size:", self.width, "x", self.height)

    def toggleWall(self, point : Point):
        (x, y) = point
        if (x, y) in self.walls:
            self.walls.remove(point)
            self.grid[y][x] = '.'
        else:
            self.walls.append(point)
            self.grid[y][x] = '#'

    def generateRandomWalls(self, wallCount : int):
        self.walls.clear()
        for i in range(wallCount):
            self.walls.append((int(random.uniform(0, self.width)), int(random.uniform(0, self.height))))
