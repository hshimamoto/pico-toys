#!/usr/bin/env python3

def to_yuv(r, g, b):
    y = 0.256788 * r + 0.504129 * g + 0.097906 * b + 16
    u = -0.148223 * r - 0.290993 * g + 0.439216 * b + 128
    v = 0.439216 * r - 0.367788 * g - 0.071427 * b + 128
    return round(y), round(u), round(v)

i = 0
for r in range(4):
    for g in range(4):
        for b in range(4):
            R, G, B = (r * 60, g * 60, b * 60)
            y, u, v = to_yuv(R, G, B)
            print("{" + f"{y}, {u}, {y}, {v}" + "}," +f" // {i} ({R}, {G}, {B})")
            i += 1
