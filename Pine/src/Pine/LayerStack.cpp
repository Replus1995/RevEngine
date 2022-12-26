#include "pinepch.h"
#include "LayerStack.h"

namespace Pine
{
	LayerStack::LayerStack()
	{
		mLayerInsert = mLayers.begin();
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer: mLayers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		mLayerInsert = mLayers.emplace(mLayerInsert, layer);
	}

	void LayerStack::PushOverlay(Layer* layer)
	{
		mLayers.emplace_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto iter = std::find(mLayers.begin(), mLayerInsert, layer);
		if (iter != mLayers.end())
		{
			mLayers.erase(iter);
			mLayerInsert--;
		}
	}

	void LayerStack::PopOverlay(Layer* layer)
	{
		auto iter = std::find(mLayerInsert, mLayers.end(), layer);
		if (iter != mLayers.end())
		{
			mLayers.erase(iter);
		}
	}

	
}
