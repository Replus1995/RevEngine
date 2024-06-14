#include "pinepch.h"
#include "UUID.h"

#include <random>
#include <unordered_map>

namespace Pine
{

static std::random_device sRandomDevice;
static std::mt19937_64 sRandomEngine(sRandomDevice());
static std::uniform_int_distribution<uint64_t> sUniformDistribution;

UUID::UUID()
	: mUUID(sUniformDistribution(sRandomEngine))
{
}

UUID::UUID(uint64_t uuid)
	: mUUID(uuid)
{
}

}