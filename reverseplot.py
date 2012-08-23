#!/usr/bin/env python
from sys import stdin
import numpy
import matplotlib.pylab as pl

fst = lambda t: t[0]
snd = lambda t: t[1]
sqr = lambda x: x**2
sqrt = lambda x: x**0.5
mean = lambda xs: sum(xs) / len(xs)
stddev = lambda xs: sqrt(mean([sqr(mean(xs)-x) for x in xs]))
stddeverr = lambda xs: stddev(xs)/sqrt(len(xs))


points = {}
for line in stdin.read().splitlines():
    # number of nodes, degree, remote degree, failrate, stretch
    n_, m_, r_, f_, s_ = line.split()
    n, m, r = map(int,(n_,m_,r_))
    f = float(f_)
    s = float(s_)
    pointid = (n,r)
    points[pointid] = points.get(pointid,[]) + [(f,s)]

failure_lines = {}
stretch_lines = {}
for (n,r), raws in points.items():
    # averages of all raw data for this point
    fs = [f for f,s in raws]
    ss = [s for f,s in raws]
    f = mean(fs)
    s = mean(ss)
    f_w = stddeverr(fs)
    s_w = stddeverr(ss)
    failure_lines[n] = failure_lines.get(n,[]) + [(r,f,f_w)]
    stretch_lines[n] = stretch_lines.get(n,[]) + [(r,s,s_w)]

for r,line in reversed(sorted(stretch_lines.items())):
    line.sort()
    xs,ys,yws = zip(*line)
    xps,yps = pl.mlab.poly_between(xs,
            [y+yw for y,yw in zip(ys,yws)],
            [y-yw for y,yw in zip(ys,yws)])
    pl.fill(xps, yps, alpha=0.4)
    pl.plot(xs, ys, '.-', label="%d"%r)
    print (xs)
    print (ys)
    print (yws)
    print ()

pl.title('Route stretch against number of remotes for different number of nodes.\nLine width is standard error for graphs with 2..5 neighbors on average.')
pl.ylabel('Route stretch')
pl.xlabel('Number of remotes')
pl.legend()
# pl.xscale('log',basex=2)
pl.show()



