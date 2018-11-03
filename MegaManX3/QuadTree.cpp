﻿#include "QuadTree.h"


QuadTree::QuadTree(UINT id, int level, int x, int y, int size)
{
	region = { x , y , x + size, y + size };
	this->level = level;
	this->nodeID = id;
	topLeft = topRight = bottomLeft = bottomRight = NULL;
}

QuadTree::QuadTree(UINT id, int level, RECT region)
{
	this->nodeID = id;
	this->level = level;
	this->region = region;
}

QuadTree::~QuadTree()
{
	if (topLeft != NULL) delete topLeft;
	topLeft = NULL;
	if (topRight != NULL) delete topRight;
	topRight = NULL;
	if (bottomLeft != NULL) delete bottomLeft;
	bottomLeft = NULL;
	if (bottomRight != NULL) delete bottomRight;
	bottomRight = NULL;
	for (auto i = staticObject.begin(); i != staticObject.end(); i++)
	{
		if ((*i) != NULL) delete *i;
		(*i) = NULL;
	}
	for (auto i = activeObject.begin(); i != activeObject.end(); i++)
	{
		if ((*i) != NULL) delete *i;
		(*i) = NULL;
	}
}

QuadTree** QuadTree::getChildNodeFromChildRect(char childRectType)
{
	switch (childRectType)
	{
	case 1:
		return &topLeft;
	case 2:
		return &topRight;
	case 3:
		return &bottomLeft;
	case 4:
		return &bottomRight;
	default:
		return NULL;
	}
}

void QuadTree::add(Object* obj, bool isStatic)
{
	if (obj == NULL) return;
	char childRectType = checkInChildRect(obj->getBound());
	if (childRectType == 0 || level >= MAX_LEVEL)			//Nếu không thể chứa vào hình chữ nhật con
	{
		if (isStatic == true)
			staticObject.push_back(obj);
		else activeObject.push_back(obj);
		obj->currentNode = this;
		return;
	}
	else
	{
		RECT r;
		QuadTree** qChild = getChildNodeFromChildRect(childRectType);
		if (*qChild == NULL)
		{
			r = getChildRect(region, childRectType);
			(*qChild) = new QuadTree(nodeID *10, level + 1, r.left, r.top, r.right - r.left);
		}
		(*qChild)->add(obj, isStatic);
	}
}

void QuadTree::deleteObject(Object* obj, bool isStatic)
{
	if (obj->currentNode == this)
	{
		Object* object;
		if (isStatic == true)
		{
			for (auto i = staticObject.begin(); i != staticObject.end(); i++)
			{
				object = (*i);
				if (object == obj)
				{
					staticObject.remove(obj);
					if (obj != NULL) delete obj;
					obj = NULL;
					return;
				}
			}
		}
		else
		{
			for (auto i = activeObject.begin(); i != activeObject.end(); i++)
			{
				object = *i;
				if (obj == object)
				{
					activeObject.remove(obj);
					if (obj != NULL) delete obj;
					obj = NULL;
					return;
				}
			}
		}
	}
	if (topLeft != NULL) topLeft->deleteObject(obj, isStatic);
	if (topRight != NULL) topRight->deleteObject(obj, isStatic);
	if (bottomLeft != NULL) bottomLeft->deleteObject(obj, isStatic);
	if (bottomRight != NULL) bottomRight->deleteObject(obj, isStatic);
}

int QuadTree::checkInChildRect(RECT rInner)
{
	RECT r = getChildRect(region, TR_TL);
	if (rectInRect(rInner, r)) {
		return TR_TL;
	}
	r = getChildRect(region, TR_TR);
	if (rectInRect(rInner, r)) {
		return TR_TR;
	}
	r = getChildRect(region, TR_BL);
	if (rectInRect(rInner, r)) {
		return TR_BL;
	}
	r = getChildRect(region, TR_BR);
	if (rectInRect(rInner, r)) {
		return TR_BR;
	}
	else {
		return TR_NONE;
	}
}

bool QuadTree::IsExistActive(Object* obj)
{
	for (auto i = activeObject.begin(); i != activeObject.end(); i++)
		if (*i == obj) return true;
	return false;
}

bool QuadTree::rectInRect(RECT rInner, RECT outerRect)
{
	return (rInner.left >= outerRect.left&&rInner.right <= outerRect.right
		&&rInner.top >= outerRect.top&&rInner.bottom <= outerRect.bottom);
}

RECT QuadTree::getChildRect(RECT rect, int typeRect)
{
	int vCenterLine = (rect.left + rect.right) / 2;
	int hCenterLine = (rect.top + rect.bottom) / 2;
	switch (typeRect)
	{
	case 1:
		return { rect.left, rect.top, vCenterLine, hCenterLine };
	case 2:
		return { vCenterLine, rect.top, rect.right, hCenterLine };
	case 3:
		return { rect.left, hCenterLine , vCenterLine, rect.bottom };
	case 4:
		return { vCenterLine, hCenterLine, rect.right, rect.bottom };
	default: 
		return { 0, 0, 0, 0 };
	}
}

bool QuadTree::IsContainable(Object* obj)
{
	//RECT của Object thuộc Rect của Quadtree nhưng không thuộc rect con thì return true
	if (rectInRect(obj->getBound(), region) && checkInChildRect(obj->getBound()) == TR_NONE)
		return true;
	else return false;
}


void QuadTree::getObjectsInNode(list<Object*>* staticList, list<Object*>* activeList)
{
	staticList->insert(staticList->end(), staticObject.begin(), staticObject.end());
	activeList->insert(activeList->end(), activeObject.begin(), activeObject.end());
}

void QuadTree::getObjectsInNode(list<Object*>* list, bool isStatic)
{
	if (isStatic == true)
		list->insert(list->end(), staticObject.begin(), staticObject.end());
	else
		list->insert(list->end(), activeObject.begin(), activeObject.end());
}

void QuadTree::getObjectsAllNode(list<Object*>* list, bool isStatic)
{
	getObjectsInNode(list, isStatic);
	if (topLeft != NULL) topLeft->getObjectsAllNode(list, isStatic);
	if (topRight != NULL) topRight->getObjectsAllNode(list, isStatic);
	if (bottomLeft != NULL) bottomLeft->getObjectsAllNode(list, isStatic);
	if (bottomRight != NULL) bottomRight->getObjectsAllNode(list, isStatic);
}

void QuadTree::getObjectsAllNode(list<Object*>* staticList, list<Object*>* activeList)
{
	getObjectsInNode(staticList, activeList);
	if (topLeft != NULL) topLeft->getObjectsAllNode(staticList, activeList);
	if (topRight != NULL) topRight->getObjectsAllNode(staticList, activeList);
	if (bottomLeft != NULL) bottomLeft->getObjectsAllNode(staticList, activeList);
	if (bottomRight != NULL) bottomRight->getObjectsAllNode(staticList, activeList);
}

void QuadTree::getObjectsFromCamera(RECT rCamera, list<Object*>* staticList, list<Object*>* activeList)
{
	int childRect = checkInChildRect(rCamera);
	if (childRect == TR_NONE || level >= MAX_LEVEL)
	{
		getObjectsAllNode(staticList, activeList);
	}
	else
	{
		getObjectsInNode(staticList, activeList);
		QuadTree** qChild = getChildNodeFromChildRect(childRect);
		if (*qChild != NULL) (*qChild)->getObjectsFromCamera(rCamera, staticList, activeList);
	}
}