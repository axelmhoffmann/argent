#include "ArchetypeCollection.h"

std::unordered_map<ArchetypeID, ag::ArchetypeCollection*> ag::ArchetypeCollection::archetypes{};
std::atomic<ArchetypeID> ag::ArchetypeCollection::nextArchetypeID(0);

ag::ArchetypeCollection::ArchetypeCollection(ComponentSet components) : ID(++nextArchetypeID)
{
	std::sort(components.begin(), components.end());
	RegisterArchetype(this);
	// Initialises the first entity's ID by mapping the archetype ID into the upper bits
	NextEntityID = ((EntityID)ID) << EPARTSIZE;

	ComponentTypes = components;
	data.resize(ComponentTypes.size());
	spawnBuffer.resize(ComponentTypes.size());
}

ag::ArchetypeCollection::~ArchetypeCollection()
{

}

void ag::ArchetypeCollection::AddComponent(byte* bytes, int i, int n, std::vector<ComponentArray>& target)
{
	target[i].insert(target[i].end(), bytes, bytes + n);
}

void ag::ArchetypeCollection::ResolveBuffers()
{
	ResolveDestroyBuffer();
	ResolveSpawnBuffer();
}

void ag::ArchetypeCollection::ResolveDestroyBuffer()
{
	/// TODO: Would probably be more cache-friendly if u swapped these loop orders
	/// TODO: Can we do this more efficiently?
	for (size_t i = 0; i < entitiesToDestroy.size(); i++)
	{
		entities.erase(std::next(entities.begin(), entitiesToDestroy.at(i)));
		for (size_t j = 0; j < ComponentTypes.size(); j++)
		{
			int componentSize = ComponentInfo::GetSize(ComponentTypes[j]);
			auto start = std::next(data[j].begin(), componentSize * entitiesToDestroy[i]);
			auto end = std::next(start, componentSize);
			data[j].erase(start, end);
		}
	}

	entitiesToDestroy.clear();
}

void ag::ArchetypeCollection::DestroyEntity(size_t index)
{
	if (index >= entities.size() || index < 0)
		return;

	entitiesToDestroy.push_back(index);
}


void ag::ArchetypeCollection::DestroyEntityByID(EntityID id)
{
	size_t index = GetIndexByID(id);
	DestroyEntity(index);
}


void ag::ArchetypeCollection::ResolveSpawnBuffer()
{
	/// TODO: reserve the amount of space required first
	// Insert data from spawn buffers into component array
	for (size_t i = 0; i < ComponentTypes.size(); i++)
	{
		data[i].insert(data[i].end(), spawnBuffer[i].begin(), spawnBuffer[i].end());
		spawnBuffer[i].clear();
	}
	// Also insert core entity data
	entities.insert(entities.end(), entitiesToSpawn.begin(), entitiesToSpawn.end());
	entitiesToSpawn.clear();
}

EntityID ag::ArchetypeCollection::GetNextID()
{
	return NextEntityID++;
}

ComponentSet ag::ArchetypeCollection::GetComponentSet()
{
	return ComponentTypes;
}

void ag::ArchetypeCollection::RegisterArchetype(ArchetypeCollection* archetype)
{
	archetypes.insert(std::pair<ArchetypeID, ArchetypeCollection*>(archetype->GetID(), archetype));
}

void ag::ArchetypeCollection::DeregisterArchetype(ArchetypeID id)
{
	archetypes.erase(id);
}

ag::ArchetypeCollection* ag::ArchetypeCollection::GetArchetypeFromEntityID(EntityID id)
{
	ArchetypeID archID = id >> EPARTSIZE;

	if (archetypes.find(archID) == archetypes.end())
		return nullptr;
		
	return archetypes[archID];
}

size_t ag::ArchetypeCollection::GetIndexByID(EntityID id)
{
	// Binary search for the entity's ID
	size_t lo = 0;
	size_t hi = entities.size() - 1;

	while (lo <= hi)
	{
		size_t mid = (lo + hi) / 2;

		if (id == entities[mid].ID)
			return mid;

		else if (id < entities[mid].ID)
			hi = mid - 1;

		else if (id > entities[mid].ID)
			lo = mid + 1;
	}

	return -1;
}
