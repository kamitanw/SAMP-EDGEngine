#include "AdvancedGDKPCH.hpp"

#include <AdvancedGDK/World/Streamer/ChunkActor.hpp>

namespace agdk::default_streamer
{

//////////////////////////////////////////////////////////////////////////////
IChunkActor::IChunkActor()
	: m_chunk{ nullptr }
{
}

//////////////////////////////////////////////////////////////////////////////
void IChunkActor::setChunk(Chunk& chunk_)
{
	m_chunk = &chunk_;
}

//////////////////////////////////////////////////////////////////////////////
void IChunkActor::setChunk(std::nullptr_t ptr_)
{
	m_chunk = nullptr;
}

//////////////////////////////////////////////////////////////////////////////
Chunk* IChunkActor::getChunk() const
{
	return m_chunk;
}

}