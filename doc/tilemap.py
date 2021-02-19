#!/usr/bin/python2
'''
@file	tilemap.py
@author	Allen Choong Chieng Hoon
@date	2013-02-04
@version	1.0.0
@copyright	Copyright 2013 Allen Choong. All rights reserved.


This is the script to convert the latitude and longitude to the 
Google Maps tile coordinate.
This script is to solve the problem of "maptiler".

Needs to bear in mind that Google Map tile coordinates origin is from
upper left.


License
========

Copyright (c) 2013, Allen Choong Chieng Hoon
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Allen Choong Chieng Hoon BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
======================


Todo
====

Convert image to tile
	Get the four points of the image, which corresponds to the four winds (north,
	south, east, west) using the latitude and longitude
	
	These four values, we can get the four points, they are
	(north, west), (north east), (south west), and (south east).
	By using these four points, we can get the four tiles (may be all in one tile).
	Then we can calculate all the tiles in between.
	The above is to get the tiles (coordinate and number of tiles)
	
	To cut the image into several tiles, we must cut it into the correct position.
	Therefore, we need to resize our image into the four points just now, and fit
	to the tile. So, just canvas the image from the (north,west). The cut the images.
'''


import sys
import math
import os.path
import Image

TILE_SIZE = 256.0

def latlngToMercator(latlng):
	'''Algorithm from
	https://developers.google.com/maps/documentation/javascript/maptypes
	https://google-developers.appspot.com/maps/documentation/javascript/examples/map-coordinates
	'''
	
	origin = (TILE_SIZE/2,TILE_SIZE/2)
	pixelsPerLngDegree = TILE_SIZE/360
	pixelsPerLngRadian = TILE_SIZE / (2*math.pi)
	
	x = origin[0] + latlng[1] * pixelsPerLngDegree
	
	temp = math.sin(math.radians(latlng[0]))
	siny = max(temp,-0.9999)
	siny = min(temp,0.9999)
	
	
	y = origin[1] + 0.5 * math.log((1+siny) / (1-siny)) * -pixelsPerLngRadian
	return x,y
	
def mercatorToLatlng(mercator):
	origin = (TILE_SIZE/2,TILE_SIZE/2)
	pixelsPerLngDegree = TILE_SIZE/360
	pixelsPerLngRadian = TILE_SIZE / (2*math.pi)
	
	lng = (mercator[0] - origin[0]) / pixelsPerLngDegree
	latRadians = (mercator[1] - origin[1]) / -pixelsPerLngRadian
	lat = math.degrees(2 * math.atan(math.exp(latRadians)) - math.pi / 2)
	return lat,lng
	
def pixelToMercator(pixel,zoom):
	numTiles = 1<<zoom
	x = pixel[0] / numTiles
	y = pixel[1] / numTiles
	return x,y
	
def mercatorToPixel(mercator,zoom):
	numTiles = 1 << zoom
	x = mercator[0] * numTiles
	y = mercator[1] * numTiles
	return x,y
	
def pixelToTile(pixels):
	x = math.floor(pixels[0]/TILE_SIZE)
	y = math.floor(pixels[1]/TILE_SIZE)
	return x,y
	
def tileToPixel(tile):
	x = tile[0]*TILE_SIZE
	y = tile[1]*TILE_SIZE
	return x,y

##########################################

def latlngToPixel(latlng,zoom):
	merc = latlngToMercator(latlng)
	pixel = mercatorToPixel(merc,zoom)
	return math.floor(pixel[0]),math.floor(pixel[1])

def pixelToLatlng(pixel,zoom):
	merc = pixelToMercator(pixel,zoom)
	latlng = mercatorToLatlng(merc)
	return latlng

def latlngToTile(latlng,zoom):
	merc = latlngToMercator(latlng)
	pixels = mercatorToPixel(merc,zoom)
	tiles = pixelToTile(pixels)
	return tiles
	
def tileToLatlng(tile,zoom):
	pixel = tileToPixel(tile)
	merc = pixelToMercator(pixel,zoom)
	latlng = mercatorToLatlng(merc)
	return latlng
	


def tileDist(latlng,zoom):
	'''This function is to get the distance of the tile (upper-left)
	from the latlng input. So that can be used to adjust the canvas
	image'''
	#Get the pixel of the upper-left of the input (latlng)
	inMerc = latlngToMercator(latlng)
	inPixel = mercatorToPixel(inMerc,zoom)
	inTile = pixelToTile(inPixel)
	
	#inTile will be used to get the pixels of the map (as canvas)
	canvasPixel = tileToPixel(inTile)
	
	#Compare the distance of the pixel
	dist = (math.floor(inPixel[0] - canvasPixel[0]),math.floor(inPixel[1] - canvasPixel[1]))
	
	#That means, the this is the distance, which will be appended to 
	# the input image, so that canvas is larger.
	# and the image is placed on the (dist) of the canvas image
	return dist
	
def tileMap(imagefile,latlng1,latlng2,zoom):
	'''Tile the input image
	@imagefile Image file name
	@latlng2 Upper left latlng tuple
	@latlng2 Lower right latlng tuple
	@zoom Zoom
	'''
	
	imagePixel1 = latlngToPixel(latlng1,zoom)
	imagePixel2 = latlngToPixel(latlng2,zoom)
	imageWidth = imagePixel2[0] - imagePixel1[0]
	imageHeight = imagePixel2[1] - imagePixel1[1]
	
	#Get the tiled pixel size
	tile1 = latlngToTile(latlng1,zoom)
	tile2 = latlngToTile(latlng2,zoom)
	tilePixel1 = tileToPixel(tile1)
	tilePixel2 = tileToPixel(tile2)
	tileWidth = tilePixel2[0] - tilePixel1[0] +TILE_SIZE
	tileHeight = tilePixel2[1] - tilePixel1[1] +TILE_SIZE
	
	#Get the position of image to the tile
	coord = (imagePixel1[0] - tilePixel1[0],imagePixel1[1] - tilePixel1[1])
	
	#Open the image and resize
	image = Image.open(imagefile)
	image = image.resize((int(imageWidth),int(imageHeight)))
	
	#Create new image based on the width and height
	tileImage = Image.new('RGBA',(int(tileWidth),int(tileHeight)))
	
	tileImage.paste(image,(int(coord[0]),int(coord[1])))
	
	#Decide how many crops needed
	cropHorz = int(tileWidth / TILE_SIZE)
	cropVert = int(tileHeight / TILE_SIZE)
	
	crops = [[ None for i in range(cropHorz) ] for j in range(cropVert) ]
	
	for j in range(cropVert):
		for i in range(cropHorz):
			crops[j][i] = tileImage.crop((int(i*TILE_SIZE), int(j*TILE_SIZE), int(i*TILE_SIZE+TILE_SIZE), int(j*TILE_SIZE+TILE_SIZE)))
			
			
			base = os.path.splitext(os.path.basename(imagefile))[0]
			crops[j][i].save(base + "_" + str(zoom) + "_" + 
				str(int(tile1[0] + i)) + "_" +
				str(int(tile1[1] + j)) + ".png")


def main(argv=None):
	if len(argv) < 7:
		print '''
Usage: tilemap.py imagefile upper_latitude left_longitude lower_latitude right_longitude zoom [zoom_until]
Make sure that the aspect ratio is compatible to the image itself.
'''
		exit(1)
	imagefile = argv[1]
	latlng1 = (float(argv[2]),float(argv[3]))
	latlng2 = (float(argv[4]),float(argv[5]))
	zoom = int(argv[6])
	zoom2 = None
	try:
		zoom2 = int(argv[7])
		if zoom2 <= zoom:
			zoom2 = None
	except:
		pass
	
	print 'Tiling the image'
	tileMap(imagefile,latlng1,latlng2,zoom)
	if zoom2:
		i= zoom+1
		while i <= zoom2:
			tileMap(imagefile,latlng1,latlng2,i)
			print 'Complete zoom %s' %(i)
			i+=1
			
	print 'Completed. Check the working directory'
	
main(sys.argv)

def test():
	latlng1 = [2.3415,111.841744] #upper left
	latlng2 = [2.340818,111.843004] #lower right
	zoom = 18
	imagefile = 'samplemap.png'
	
	tileMap(imagefile,latlng1,latlng2,zoom)
	
