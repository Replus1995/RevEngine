#include "Rev/Core/LayerStack.h"

namespace Rev
{
	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer: mLayers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
		mLayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		mLayers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto iter = std::find(mLayers.begin(), mLayers.begin() + mLayerInsertIndex, layer);
		if (iter != mLayers.end())
		{
			layer->OnDetach();
			mLayers.erase(iter);
			mLayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto iter = std::find(mLayers.begin() + mLayerInsertIndex, mLayers.end(), overlay);
		if (iter != mLayers.end())
		{
			overlay->OnDetach();
			mLayers.erase(iter);
		}
	}

	void LayerStack::PopAll()
	{
		for (auto layer : mLayers)
		{
			layer->OnDetach();
			delete layer;//Temp
		}
		mLayers.clear();
		mLayerInsertIndex = 0;

	}

	
}
