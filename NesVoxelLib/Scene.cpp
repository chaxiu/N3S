#include "stdafx.h"
#include "Scene.hpp"
#include "Camera.hpp"
#include "Editor.hpp"

XMVECTOR mouseVector;
XMFLOAT3 zIntersect;

Vector2D mousePixelCoordinates;

Camera mainCamera;

bool mouseCaptured = false;

bool selectionClickedOn = false;
bool draggingSelection = false;
MouseModifier modifier = no_mod;
MouseFunction mouseFunction = no_func;
Vector2D originMousePixelCoordinates;

int sel_top, sel_left, sel_bottom, sel_right;

bool edittingIn3d = false;

bool isSpriteIn2dRect(int top, int left, int bottom, int right, int x, int y)
{
	if ((x >= left && x < right) || (x + 8 >= left && x + 8 < right))
	{
		if ((y >= top && y < bottom) || (y + 8 >= top && y + 8 < bottom))
		{
			return true;
		}
	}
	return false;
}

Scene::Scene()
{
	// Set camera to default position
	mainCamera.SetPosition(0, 0, -2);
	// Clear BG with all "blank" (-1) sprites
	for (int i = 0; i < sceneWidth * sceneHeight; i++)
	{
		bg[i] = { -1, 0, false, false };	
	}
	selection = make_shared<Selection>();
	displaySelection = make_shared<Selection>();
}

Scene::Scene(shared_ptr<PpuSnapshot> snapshot)
{
	// Set camera to default position
	mainCamera.SetPosition(0, 0, -2);
	// Clear BG with all "blank" (-1) sprites
	for (int i = 0; i < sceneWidth * sceneHeight; i++)
	{
		bg[i] = { -1, 0, false, false };
	}
	selection = make_shared<Selection>();
	displaySelection = make_shared<Selection>();
	// Create scene from snapshot
	// Grab all OAM
	for (int i = 0; i < snapshot->sprites.size(); i++)
	{
		OamSprite s = snapshot->sprites[i];
		int tile = snapshot->getTrueOamTile(i);
		int id = N3sApp::virtualPatternTable->getSprite(s.tile)->defaultMesh->id; // lol
		if (id >= 0)
		{
			SceneSprite ss;
			ss.x = s.x;
			ss.y = s.y;
			ss.palette = s.palette;
			ss.meshNum = id;
			ss.mirrorH = s.hFlip;
			ss.mirrorV = s.vFlip;
			sprites.push_back(ss);
		}
	}
	// Grab all NT
	for (int y = 0; y < sceneHeight; y++)
	{
		for (int x = 0; x < sceneWidth; x++)
		{
			NameTableTile t = snapshot->background.getTile(x, y, 0);
			int index = getArrayIndexFromXY(x, y, sceneWidth);
			int tile = snapshot->getTrueNTTile(index);
			shared_ptr<VirtualSprite> s = N3sApp::virtualPatternTable->getSprite(tile);
			int id = s->defaultMesh->id;
			// Add to nametable index
			SceneSprite ss;
			ss.x = 0;
			ss.y = 0;
			ss.palette = t.palette;
			ss.meshNum = id;
			ss.mirrorH = false;
			ss.mirrorV = false;
			bg[index] = ss;
		}
	}
	// Grab palette
	for (int i = 0; i < 8; i++)
		palettes[i] = snapshot->palette;
}

bool Scene::update(bool mouseAvailable)
{
	// Update camera position
	if (InputState::keyboardMouse->mouseButtons[right_mouse].state > 0)
	{
		float xRot = InputState::keyboardMouse->mouseDeltaX / 3;
		float yRot = InputState::keyboardMouse->mouseDeltaY / 3;
		mainCamera.AdjustRotation(xRot, 0.0f, yRot);
	}
	// Update camera math
	mainCamera.Render();
	// Calculate mouse vector and z-intersect
	mouseVector = N3s3d::getMouseVector(&mainCamera, InputState::keyboardMouse->mouseX, InputState::keyboardMouse->mouseY);
	zIntersect = N3s3d::getZIntersection(&mainCamera, InputState::keyboardMouse->mouseX, InputState::keyboardMouse->mouseY);
	mousePixelCoordinates = getCoordinatesFromZIntersection(zIntersect);
	return updateMouseActions(mouseAvailable);
}

void Scene::render(bool renderBackground, bool renderOAM)
{
	// Enable depth buffer
	N3s3d::setDepthBufferState(true);
	// Update camera math
	N3s3d::updateMatricesWithCamera(&mainCamera);
	// Update palette in video card
	palettes[selectedPalette].updateShaderPalette();
	// Render background, if enabled
	if (renderBackground)
	{
		for (int y = 0; y < sceneHeight; y++)
		{
			int yCalc = y * sceneWidth;
			for (int x = 0; x < sceneWidth; x++)
			{
				SceneSprite sprite = bg[yCalc + x];
				// Only render non-empty spots, which are 0 or greater
				if (sprite.meshNum >= 0)
				{
					int i = getArrayIndexFromXY(x, y, sceneWidth);
					if (displaySelection->selectedBackgroundIndices.count(i) > 0 || highlight.getHighlightedNT() == i)
					{
						N3s3d::setDepthStencilState(true, true, false);
						N3sApp::gameData->meshes[sprite.meshNum]->render(x * 8, y * 8, sprite.palette, sprite.mirrorH, sprite.mirrorV, { 0,0,0,0 });
						N3s3d::setDepthStencilState(true, false, false);
					}
					// See if this tile is selected or highlighted
					N3sApp::gameData->meshes[sprite.meshNum]->render(x * 8, y * 8, sprite.palette, sprite.mirrorH, sprite.mirrorV, { 0,0,0,0 });
				}
			}
		}
	}
	// Render OAM, if enabled
	if (renderOAM)
	{
		for (int i = 0; i < sprites.size(); i++)
		{
			SceneSprite s = sprites[i];
			// See if sprite is highlighted and write to stencil buffer if so
			if (displaySelection->selectedSpriteIndices.count(i) > 0 || highlight.getHighlightedOAM() == i)
			{
				N3s3d::setDepthStencilState(true, true, false);
				N3sApp::gameData->meshes[s.meshNum]->render(s.x, s.y, s.palette, s.mirrorH, s.mirrorV, { 0, 0, 0, 0 });
				N3s3d::setDepthStencilState(true, false, false);
			}
			else
			{
				N3sApp::gameData->meshes[s.meshNum]->render(s.x, s.y, s.palette, s.mirrorH, s.mirrorV, { 0, 0, 0, 0 });
			}
		}
	}
	// Render highlight selections / mouse hover
	N3s3d::setDepthStencilState(false, false, true);
	N3s3d::setGuiProjection();
	Overlay::setColor(1.0f, 1.0f, 1.0f, 0.3f);
	Overlay::drawRectangle(0, 0, 1920, 1080); // TODO: How do I get actual screen size again..?
	// Draw selection box, if currently dragging selection
	if (draggingSelection)
	{
		N3s3d::setDepthBufferState(false);
		Overlay::setColor(1.0f, 1.0f, 1.0f, 0.3f);
		// Draw differently based on 2D or 3D editing mode
		if (edittingIn3d)
		{
			Overlay::drawRectangle(sel_left, sel_top, sel_right - sel_left, sel_bottom - sel_top);
		}
		else
		{
			// Draw onto z-axis in pixel-space
			N3s3d::updateMatricesWithCamera(&mainCamera);
			Overlay::drawRectangleInScene(sel_left, sel_top, 0, sel_right - sel_left, sel_bottom - sel_top);
		}
	}
	// Render selection boxes around OAM and NT
	N3s3d::setDepthBufferState(false);
	N3s3d::updateMatricesWithCamera(&mainCamera);
	N3s3d::setRasterFillState(false);
	displaySelection->render(&sprites);
	N3s3d::setRasterFillState(true);
}

void Scene::renderOverlays(bool drawBackgroundGrid, bool drawOamHighlights)
{
	N3s3d::setShader(overlay);
	N3s3d::setDepthBufferState(false);
	N3s3d::setRasterFillState(false);
	// Update camera math (was probably left at GUI projection after scene rendering)
	N3s3d::updateMatricesWithCamera(&mainCamera);
	// Render background grid, if enabled
	if (drawBackgroundGrid)
	{
		Overlay::setColor(1.0f, 0.0f, 0.0f, 0.1f);
		Overlay::drawNametableGrid(0, 0);
		Overlay::drawNametableGrid(32, 0);
		Overlay::drawNametableGrid(0, 30);
		Overlay::drawNametableGrid(32, 30);
	}
	// Draw OAM highlights, if enabled
	if (drawOamHighlights)
	{
		Overlay::setColor(1.0f, 1.0f, 1.0f, 1.0f);
		for each(SceneSprite s in sprites)
		{
			Overlay::drawSpriteSquare(s.x, s.y);
		}
	}
}

void Scene::setBackgroundSprite(int x, int y, SceneSprite sprite)
{
	bg[y * sceneWidth + x] = sprite;
}

void Scene::addOAMSprite(SceneSprite s)
{
	sprites.push_back(s);
}

void Scene::createSceneFromCurrentSnapshot()
{
	int scrollX = N3sApp::snapshot->scrollSections[0].x;
	int scrollY = N3sApp::snapshot->scrollSections[0].y;
	int nameTable = N3sApp::snapshot->scrollSections[0].nameTable;
	// todo: adjust scroll x and y by nametable selection

	// Grab all background tiles
	for (int y = 0; y < sceneHeight; y++)
	{
		int yName = floor(y / 30);
		int yCalc = y * sceneWidth;
		for (int x = 0; x < sceneWidth; x++)
		{
			int xName = floor(x / 32);
			NameTableTile t = N3sApp::snapshot->background.getTile(x, y, yName + xName);
			SceneSprite s = { t.tile, t.palette, false, false }; // NOPE gotta get the mesh #
		}
	}
}

N3sPalette * Scene::getSelectedPalette()
{
	return &palettes[selectedPalette];
}

void Scene::selectNextPalette()
{
	selectedPalette++;
	if (selectedPalette > 7)
		selectedPalette = 0;
}

void Scene::selectPreviousPalette()
{
	selectedPalette--;
	if (selectedPalette < 0)
		selectedPalette = 7;
}

void Scene::updateHighlight2d(Vector2D mouse, bool highlightOAM, bool highlightNametable)
{
	// Clear previous highlight
	highlight.clear();
	// See if any OAM sprites intersect selection
	if (highlightOAM)
	{
		for (int i = 0; i < sprites.size(); i++)
		{
			SceneSprite s = sprites[i];
			if (mouse.x >= s.x && mouse.x < s.x + 8 && mouse.y >= s.y && mouse.y < s.y + 8)
				highlight.highlightedSpriteIndices.push_back(i);
		}
	}
	// See if any part of the background intersects selection
	if (highlightNametable)
	{
		if (mouse.x >= 0 && mouse.x < scenePixelWidth && mouse.y >= 0 && mouse.y < scenePixelHeight)
			highlight.highlightedBackgroundIndex = floor(mouse.y / 8) * 64 + floor(mouse.x / 8);
	}
	// Set the index
	if (highlight.highlightedBackgroundIndex >= 0 || highlight.highlightedSpriteIndices.size() > 0)
		highlight.selectedIndex = 0;
}

bool Scene::updateMouseActions(bool mouseAvailable)
{
	// If mouse has moved and is available, calculate highlighted items
	if (InputState::keyboardMouse->hasMouseMoved())
		updateHighlight2d(mousePixelCoordinates, true, true);
	// Check left mouse
	if (mouseAvailable || mouseCaptured)
	{
		MouseState state = InputState::keyboardMouse->mouseButtons[left_mouse].state;
		if (state > 0)
			mouseCaptured = true;
		else
			mouseCaptured = false;
		int mouseX = InputState::keyboardMouse->mouseX;
		int mouseY = InputState::keyboardMouse->mouseY;
		if (mouseCaptured)
		{
			// See if this is a new click
			if (state == clicked)
			{
				// See if OAM or NT is highlighted at all
				if (highlight.anythingHighlighted())
				{
					// See if the current selection is highlighted
					if(selection->selectedSpriteIndices.count(highlight.getHighlightedOAM()) > 0 ||
					   selection->selectedBackgroundIndices.count(highlight.getHighlightedNT() > 0))
						selectionClickedOn = true;
				}
				// Capture mod key
				if (InputState::functions[selection_add].active && InputState::functions[selection_remove].active)
					modifier = mod_intersect;
				else if (InputState::functions[selection_add].active)
					modifier = mod_add;
				else if (InputState::functions[selection_remove].active)
					modifier = mod_remove;
				else
				{
					modifier = no_mod;
				}
				// Capture Z-intersect at click
				originMousePixelCoordinates = getCoordinatesFromZIntersection(zIntersect);
			}
			else if (state == pressed)
			{
				// Clear previous selection when appropriate
				if(modifier == no_mod)
					selection->clear();
				// Check for highlight and add to selection
				if (highlight.getHighlightedOAM() >= 0)
				{
					if (modifier == no_mod || modifier == mod_add)
						selection->selectedSpriteIndices.insert(highlight.getHighlightedOAM());
					else if (modifier == mod_remove)
						selection->selectedSpriteIndices.erase(highlight.getHighlightedOAM());
				}
				else if (highlight.getHighlightedNT() >= 0)
				{
					if (modifier == no_mod || modifier == mod_add)
						selection->selectedBackgroundIndices.insert(highlight.getHighlightedNT());
					else if (modifier == mod_remove)
						selection->selectedBackgroundIndices.erase(highlight.getHighlightedNT());
				}
			}
			else if (state == dragging)
			{
				// If we highlighted any part of our selection, then mouse dragging should move the whole thing
				if (selectionClickedOn)
				{
					// Create function within selection to do this, that makes the most sense
				}
				// If a modifier was held down, or no part of selection was highlighted, start drag selection
				if (!selectionClickedOn)
				{
					if(modifier == no_mod)
						selection->clear();
					draggingSelection = true;
					// Calculate the new selection area
					int x, y, startX, startY;
					// Branch based on 2D or 3D editing mode
					if (edittingIn3d)
					{
						// Get X&Y or beginning and end of drag in screen-space
						x = mouseX;
						y = mouseY;
						startX = InputState::keyboardMouse->mouseButtons[left_mouse].actionXStart;
						startY = InputState::keyboardMouse->mouseButtons[left_mouse].actionYStart;
					}
					else
					{
						// Get X&Y or beginning and end of drag in game-space
						x = mousePixelCoordinates.x;
						y = mousePixelCoordinates.y;
						startX = originMousePixelCoordinates.x;
						startY = originMousePixelCoordinates.y;
					}
					// Set sides of selection
					if (x > startX)
					{
						sel_right = x;
						sel_left = startX;
					}
					else
					{
						sel_right = startX;
						sel_left = x;
					}
					if (y > startY)
					{
						sel_top = startY;
						sel_bottom = y;
					}
					else
					{
						sel_top = y;
						sel_bottom = startY;
					}
					// Get all items current in the selection box in their own selection
					shared_ptr<Selection> tempSelection = make_shared<Selection>();
					if (true) // TODO add actual options
					{
						for(int i = 0; i < sprites.size(); i++)
						{
							SceneSprite s = sprites[i];
							if (isSpriteIn2dRect(sel_top, sel_left, sel_bottom, sel_right, sprites[i].x, sprites[i].y))
								tempSelection->selectedSpriteIndices.insert(i);
						}
					}
					if (true)
					{
						for (y = 0; y < sceneHeight; y++)
							for (x = 0; x < sceneWidth; x++)
								if (isSpriteIn2dRect(sel_top, sel_left, sel_bottom, sel_right, x * 8, y * 8))
								{
									int i = getArrayIndexFromXY(x, y, sceneWidth);
									if(bg[i].meshNum >= 0)
										tempSelection->selectedBackgroundIndices.insert(i);
								}
					}
					// Set new display selection to be some combination of the two, depending on modifier key
					switch (modifier)
					{
					case mod_add:
						displaySelection = Selection::getUnion(selection, tempSelection);
						break;
					case mod_remove:
						displaySelection = Selection::getSubtraction(selection, tempSelection);
						break;
					case mod_intersect:
						displaySelection = Selection::getIntersection(selection, tempSelection);
						break;
					default:	// No modifier
						displaySelection = tempSelection;
						break;
					}
				}
			}
		}
	}
	if(!mouseAvailable || !mouseCaptured)
	{
		// Set display selection to the new, real selection if we just stopped dragging
		if(draggingSelection)
			selection = displaySelection;
		mouseFunction = no_func;
		modifier = no_mod;
		draggingSelection = false;
		selectionClickedOn = false;
	}
	// The displayed selection is just the current selection if we're not dragging
	// (This might be redundant if we've just finished dragging, oh well)
	if (!draggingSelection)
		displaySelection = selection;
	return true;
}

Vector2D Scene::getCoordinatesFromZIntersection(XMFLOAT3 zIntersect)
{
	Vector2D mouse;
	float xAtIntersect = zIntersect.x;
	float yAtIntersect = zIntersect.y;
	// Normalize top-left of all screens to 0,0
	xAtIntersect += 1.0f;
	yAtIntersect -= 1.0f;
	// Divide each by full size of scene
	xAtIntersect /= sceneDXWidth;
	yAtIntersect /= sceneDXHeight;
	// Get "pixel" position of X
	mouse.x = floor(scenePixelWidth * xAtIntersect);
	// Get Y, but flip it first (since negative = positive in NES screenspace)
	mouse.y = floor(scenePixelHeight * (yAtIntersect * -1));
	return mouse;
}

void Highlight::clear()
{
	highlightedSpriteIndices.clear();
	highlightedBackgroundIndex = -1;
	selectedIndex = -1;
}

int Highlight::getHighlightedOAM()
{
	if (selectedIndex >= 0 && selectedIndex < highlightedSpriteIndices.size())
		return highlightedSpriteIndices[selectedIndex];
	else
		return -1;
}

int Highlight::getHighlightedNT()
{
	if (selectedIndex == highlightedSpriteIndices.size())
		return highlightedBackgroundIndex;
	else
		return -1;
}

bool Highlight::anythingHighlighted()
{
	if (selectedIndex >= 0)
		return true;
	else
		return false;
}

void Selection::clear()
{
	selectedSpriteIndices.clear();
	selectedBackgroundIndices.clear();
}

shared_ptr<Selection> Selection::getUnion(shared_ptr<Selection> a, shared_ptr<Selection> b)
{
	shared_ptr<Selection> temp = make_shared<Selection>();
	for each(int i in a->selectedSpriteIndices)
		temp->selectedSpriteIndices.insert(i);
	for each(int i in b->selectedSpriteIndices)
		temp->selectedSpriteIndices.insert(i);
	for each(int i in a->selectedBackgroundIndices)
		temp->selectedBackgroundIndices.insert(i);
	for each(int i in b->selectedBackgroundIndices)
		temp->selectedBackgroundIndices.insert(i);
	return temp;
}

shared_ptr<Selection> Selection::getSubtraction(shared_ptr<Selection> first, shared_ptr<Selection> second)
{
	shared_ptr<Selection> temp = make_shared<Selection>();
	// Get all selections for first sets
	temp->selectedSpriteIndices = first->selectedSpriteIndices;
	temp->selectedBackgroundIndices = first->selectedBackgroundIndices;
	// Remove any that are also in the second sets
	for each(int i in second->selectedSpriteIndices)
		temp->selectedSpriteIndices.erase(i);
	for each(int i in second->selectedBackgroundIndices)
		temp->selectedBackgroundIndices.erase(i);
	return temp;
}

shared_ptr<Selection> Selection::getIntersection(shared_ptr<Selection> a, shared_ptr<Selection> b)
{
	shared_ptr<Selection> temp = make_shared<Selection>();
	// Add sprite and nt indices if both selections have them
	for each(int i in a->selectedSpriteIndices)
	{
		if(b->selectedSpriteIndices.count(i) > 0)
			temp->selectedSpriteIndices.insert(i);
	}
	for each(int i in b->selectedBackgroundIndices)
	{
		if (b->selectedBackgroundIndices.count(i) > 0)
			temp->selectedBackgroundIndices.insert(i);
	}
	return temp;
}

void Selection::render(vector<SceneSprite> * sprites)
{
	// Render all OAM highlights
	Overlay::setColor(0.0f, 1.0f, 0.0f, 1.0f);
	for each(int i in selectedSpriteIndices)
	{
		SceneSprite s = sprites->at(i);
		Overlay::drawSpriteSquare(s.x, s.y);
	}
	// Render all NT highlights
	Overlay::setColor(1.0f, 0.0f, 0.0f, 1.0f);
	for each(int i in selectedBackgroundIndices)
	{
		Vector2D pos = unwrapArrayindex(i, sceneWidth);
		Overlay::drawSpriteSquare(pos.x * 8, pos.y * 8);
	}
}