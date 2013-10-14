#!/opt/local/bin/python
from numpy import loadtxt, reshape, uint8
from Image import fromarray

imdata = loadtxt('Calcium_image.dat')
pxmx = max(imdata[:,2])
pixdata = reshape(imdata[:,2], (640, 640))
im = fromarray((pixdata/pxmx*255).astype(uint8))
im.save('Ca.png')

imdata = loadtxt('CalciumFluoride_image.dat')
pxmx = max(imdata[:,2])
pixdata = reshape(imdata[:,2], (640, 640))
im = fromarray((pixdata/pxmx*255).astype(uint8))
im.save('CaF.png')
