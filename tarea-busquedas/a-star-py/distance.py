#!/usr/bin/env python
# -*- coding: utf-8 -*-
from math import sqrt

Point = tuple[int, int]

def euclidean_dist(first : Point, second : Point) -> float:
    _x = first[0] - second[0]
    _y = first[1] - second[1]
    return sqrt(_x*_x + _y*_y)

def manhattan_dist(first : Point, second : Point) -> int:
    _x = abs(first[0] - second[0])
    _y = abs(first[1] - second[1])
    return _x + _y
