#include "stdafx.h"
#include "RenderBatch.hpp"

RenderBatch::RenderBatch(shared_ptr<GameData> gameData, shared_ptr<PpuSnapshot> snapshot, shared_ptr<VirtualPatternTable> vPatternTable)
	:	gameData(gameData), snapshot(snapshot), vPatternTable(vPatternTable)
{
	computeSpritesOAM();
	processMeshesOAM();
	computeSpritesNametable();
	processMeshesNT();
	processStencilGroups();
	batchDrawCallsOAM();
	batchDrawCallsNT();
}

void RenderBatch::computeSpritesOAM()
{
	// Grab all (true) sprites from the snapshot
	for each (OamSprite s in snapshot->sprites)
	{
		int x = s.x;
		int y = s.y;
		int tile = s.tile;
		int palette = s.palette;
		// Make sure palette isn't out of bounds for any reason
		if (palette > 7)
			palette = 7;
		// Make sure sprite is on the screen and not garbage data
		if (s.x > 0 && s.x < 256 && s.y > 0 && s.y < 240)
		{
			// Branch based on 16x8 mode
			if (snapshot->ctrl.spriteSize16x8)
			{
				// In 8x16, pattern table selection is specified by first bit.
				// Since you can only select even (in base 0) tiles it is free for this purpose.
				// Actual tile selection is bits 1-7.
				int patternSelection = tile & 1;
				// Nestopia tends to give tiles by absolute value, but seemingly only some of the time?
				// So we need to correct to be sure
				if (patternSelection && tile < 256)
					tile += 256;
				else if (!patternSelection && tile > 255)
					tile -= 256;
				if (patternSelection)
					tile--;
				// Flipped sprites in 8x16 also swap places on the Y axis
				if (s.vFlip)
					y += 8;
				// Get true tile #
				shared_ptr<VirtualSprite> vSprite = vPatternTable->getSprite(tile);
				shared_ptr<VirtualSprite> vSprite2 = vPatternTable->getSprite(tile + 1);
				// Build and add the first computed sprite
				ComputedSprite cs;
				cs.virtualSprite = vSprite;
				cs.mesh = nullptr;
				cs.position = { x, y };
				cs.palette = palette;
				cs.mirrorH = s.hFlip;
				cs.mirrorV = s.vFlip;
				computedSprites.push_back(cs);
				// Create the second sprite, which swaps place with vertical flip
				ComputedSprite cs2 = cs;
				if (s.vFlip)
					cs2.position.y -= 8;
				else
					cs2.position.y += 8;
				cs2.virtualSprite = vSprite2;
				computedSprites.push_back(cs2);
			}
			else
			{
				// Select tile based on pattern table in CTRL register
				if (tile > 255 && !snapshot->getOAMSelectAtScanline(y))
				{
					tile -= 256;
				}
				else if (tile < 256 && snapshot->getOAMSelectAtScanline(y))
				{
					tile += 256;
				}
				shared_ptr<VirtualSprite> vSprite = vPatternTable->getSprite(tile);
				// Build and add the computed sprite
				ComputedSprite cs;
				cs.virtualSprite = vSprite;
				cs.mesh = nullptr;
				cs.position = { x, y };
				cs.palette = palette;
				cs.mirrorH = s.hFlip;
				cs.mirrorV = s.vFlip;
				computedSprites.push_back(cs);
			}
		}
	}
}

void RenderBatch::computeSpritesNametable()
{
	// Grab all true tile numbers and palettes from snapshot nametable
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 60; y++)
		{
			NameTableTile n = snapshot->background.getTile(x, y, 0);
			if (snapshot->scrollSections[0].patternSelect)
				n.tile += 256;
			nametable.tiles[x][y].virtualSprite = vPatternTable->getSprite(n.tile);
			nametable.tiles[x][y].palette = n.palette;
		}
	}
}

void RenderBatch::processMeshesOAM()
{
	// Process meshes (FOR NOW ASSUMING DEFAULT)
	for(int i = 0; i < computedSprites.size(); i++)
	{
		computedSprites[i].mesh = computedSprites[i].virtualSprite->defaultMesh;
	}
}

void RenderBatch::processMeshesNT()
{
	// Process meshes (FOR NOW ASSUMING DEFAULT)
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 60; y++)
		{
			nametable.tiles[x][y].mesh = nametable.tiles[x][y].virtualSprite->defaultMesh;
		}
	}
}

void RenderBatch::processStencilGroups()
{
	int previousColorIndex = -1;
	for (int i = 0; i < computedSprites.size(); i++)	// Assuming this maintains insertion order...
	{
		ComputedSprite s = computedSprites[i];
		int newColorIndex = s.palette + s.mesh->outlineColor;
		// See if this mesh exists and has any outlines
		if (s.mesh->meshExists && s.mesh->outlineColor >= 0)
		{
			// Create bool, assuming same group
			bool sameGroup = true;
			// Check all grouping restraints
			if (gameData->oamGrouping == continous_samecolor)
			{
				// See if the color is the same as before
				if (newColorIndex != previousColorIndex)
					sameGroup = false;
			}
			// If grouping is not the same, increment stencil number
			if (!sameGroup)
				currentStencilNumber++;
			computedSprites[i].stencilGroup = currentStencilNumber;
		}
		else	// If this has no outline, 
		{
			computedSprites[i].stencilGroup = -1;
			previousColorIndex = -1;
		}
	}
}

void RenderBatch::batchDrawCallsOAM()
{
	for each(ComputedSprite s in computedSprites)
	{
		if (s.mesh->meshExists)
		{
			// Generate palette draw call and add to list
			PaletteDrawCall pDraw;
			pDraw.palette = s.palette;
			pDraw.mirrorH = s.mirrorH;
			pDraw.mirrorV = s.mirrorV;
			pDraw.mesh = s.mesh->mesh;
			pDraw.stencilGroup = s.stencilGroup;
			// Adjust position based on mirroring
			pDraw.position = s.position.convertToWorldSpace();
			if (pDraw.mirrorH)
				pDraw.position.x += 8 * pixelSizeW;
			if (pDraw.mirrorV)
				pDraw.position.y -= 8 * pixelSizeW;
			paletteDrawCalls[s.palette].push_back(pDraw);
			// If outlining, add outline draw call to batch, based on stencil group
			if (s.stencilGroup >= 0)
			{
				// See if batch already exists, create if needed
				if (outlineBatches.count(s.stencilGroup) == 0)
				{
					shared_ptr<OutlineBatch> batch = make_shared<OutlineBatch>();
					batch->palette = s.palette;
					batch->color = s.mesh->outlineColor;
					batch->stencilGroup = s.stencilGroup;
					outlineBatches[s.stencilGroup] = batch;
				}
				// Push outline to appropriate batch
				outlineBatches[s.stencilGroup]->outlines.push_back({ s.mesh->outlineMesh, pDraw.position, s.mirrorH, s.mirrorV });
			}
		}
	}
}

void RenderBatch::batchDrawCallsNT()
{
	for (ScrollSection section : snapshot->scrollSections)
	{
		// Get offset of top-left pixel within top-left sprite referenced
		int xOffset = section.x % 8;
		int yOffset = section.y % 8;
		// Get size of section
		int sectionHeight = section.bottom - section.top + 1;
		// Render rows based on section size and yOffset
		int topRowHeight = 8 - yOffset;
		batchRow(section.top, topRowHeight, xOffset, yOffset, section.x, section.y, section.nameTable, section.patternSelect);
		int yPositionOffset = topRowHeight;
		// Render rest of rows, depending on how many there are
		if (sectionHeight <= 8)
		{
			if (sectionHeight > topRowHeight)
			{
				int bottomRowHeight = sectionHeight - topRowHeight;
				batchRow(section.top + yPositionOffset, bottomRowHeight, xOffset, 0, section.x, section.y + yPositionOffset, section.nameTable, section.patternSelect);
			}
		}
		else
		{
			// Render all full rows
			int fullRows = floor((sectionHeight - topRowHeight) / 8);
			for (int i = 0; i < fullRows; i++)
			{
				batchRow(section.top + yPositionOffset, 8, xOffset, 0, section.x, section.y + yPositionOffset, section.nameTable, section.patternSelect);
				yPositionOffset += 8;
			}
			if (yOffset != 0)
			{
				int bottomRowHeight = (sectionHeight - topRowHeight) % 8;
				batchRow(section.top + yPositionOffset, bottomRowHeight, xOffset, 0, section.x, section.y + yPositionOffset, section.nameTable, section.patternSelect);
			}
		}
	}
}

void RenderBatch::batchRow(int y, int height, int xOffset, int yOffset, int nametableX, int nametableY, int nameTable, bool patternSelect)
{
	int x = 0;
	int tileX = floor(nametableX / 8);
	int tileY = floor(nametableY / 8);
	int tile;
	int palette;
	Crop crop = { yOffset, xOffset, 8 - yOffset - height, 0 };
	// Branch based on whether or not there is any X offset / partial sprite
	if (xOffset > 0)
	{
		int i = 0;
		// Batch partial first sprite
		ComputedSprite s = nametable.tiles[tileX][tileY];
		batchMeshCropped(s, crop);
		x += 8 - xOffset;
		i++;
		// Render middle sprites
		crop.left = 0;
		for (i; i < 32; i++)
		{
			int tileXMod = (x + i) % 64;
			s = nametable.tiles[tileXMod][tileY];
			batchMeshCropped(s, crop);
			x += 8;
		}
		// Render partial last sprite
		crop.right = 8 - xOffset;
		int tileXMod = (x + i) % 64;
		s = nametable.tiles[tileXMod][tileY];
		batchMeshCropped(s, crop);
	}
	else
	{
		for (int i = 0; i < 32; i++)
		{
			int tileXMod = (x + i) % 64;
			ComputedSprite s = nametable.tiles[tileXMod][tileY];
			// Generate palette draw call and add to list, if mesh exists
			if (s.mesh->meshExists)
			{
				PaletteDrawCall pDraw;
				pDraw.palette = s.palette;
				pDraw.mirrorH = false;
				pDraw.mirrorV = false;
				pDraw.mesh = s.mesh->mesh;
				pDraw.stencilGroup = s.stencilGroup;
				pDraw.position = { x * pixelSizeW, y * pixelSizeW };
				paletteDrawCalls[s.palette].push_back(pDraw);
			}
			x += 8;
		}
	}
}

void RenderBatch::batchMeshCropped(ComputedSprite s, Crop crop)
{
}

void RenderBatch::render(shared_ptr<Camera> camera)
{
	// Prep scene
	N3s3d::setShader(color);
	camera->Render();
	N3s3d::updateMatricesWithCamera(camera);
	// Update palette in shader
	snapshot->palette.updateShaderPalette();
	// Draw all palette meshes, which are grouped by palette for optimized rendering
	for (int i = 0; i < 8; i++)
	{
		// Check if this palette has any calls (don't bother updating cbuffers if not)
		if (paletteDrawCalls[i].size() > 0)
		{
			// Select palette in shader and draw all
			N3s3d::selectPalette(i);
			for each(PaletteDrawCall dC in paletteDrawCalls[i])
			{
				// Update cbuffers as needed (N3s3D class weeds out redundant calls)
				N3s3d::updateWorldMatrix(dC.position.x, dC.position.y, 0.0f);
				N3s3d::updateMirroring(dC.mirrorH, dC.mirrorV);
				// Set the stencil mode, if outlining
				if (dC.stencilGroup >= 0)
					N3s3d::setStencilingState(stencil_write, dC.stencilGroup);
				else
					N3s3d::setStencilingState(stencil_nowrite, 0);
				// Render the mesh
				N3s3d::renderMesh(&dC.mesh);
			}
		}
	}
	// Draw all outlines, if there are any
	if (outlineBatches.size() > 0)
	{
		N3s3d::setShader(outline);
		N3s3d::updateMatricesWithCamera(camera);
		for each(auto kv in outlineBatches)
		{
			int stencilNumber = kv.first;
			shared_ptr<OutlineBatch> batch = kv.second;
			N3s3d::selectOutlinePalette(batch->palette, batch->color);
			N3s3d::setStencilingState(stencil_mask, batch->stencilGroup);
			for each(OutlineDrawCall oDC in batch->outlines)
			{
				N3s3d::updateWorldMatrix(oDC.position.x, oDC.position.y, 0.0f);
				N3s3d::updateMirroring(oDC.mirrorH, oDC.mirrorV);
				N3s3d::renderMesh(&oDC.mesh);
			}
		}
	}
}