#include "image_manager.h"
#include <SFML/Network.hpp>

namespace ygo {

ImageManager imageManager;

bool ImageManager::Initial() {
	tCover[0] = LoadTexture("textures/cover");
	tCover[1] = LoadTexture("textures/cover2");
	tUnknown = LoadTexture("textures/unknown");
	tAct = LoadTexture("textures/act");
	tAttack = LoadTexture("textures/attack");
	tChain = LoadTexture("textures/chain");
	tNegated = LoadTexture("textures/negated");
	tNumber = LoadTexture("textures/number");
	tLPBar = LoadTexture("textures/lp");
	tLPFrame = LoadTexture("textures/lpf");
	tMask = LoadTexture("textures/mask");
	tEquip = LoadTexture("textures/equip");
	tTarget = LoadTexture("textures/target");
	tLim = LoadTexture("textures/lim");
	tHand[0] = LoadTexture("textures/f1");
	tHand[1] = LoadTexture("textures/f2");
	tHand[2] = LoadTexture("textures/f3");
	tBackGround = LoadTexture("textures/bg");
	tBackGround2 = LoadTexture("textures/bg2"); 
	tField = LoadTexture("textures/field2");
	tFieldTransparent = LoadTexture("textures/field-transparent");
	for (int i = 0; i < 4; ++i)
		tAvatar[i] = NULL;
	return true;
}
irr::video::ITexture* ImageManager::LoadTexture(const char* image) {
	ITexture* texture;
	char file[256];
	strncpy(file, image, sizeof(file));
	strncat(file,".png", sizeof(file));
	texture = driver->getTexture(file);
	if (texture == NULL)
	{
		strncpy(file, image, sizeof(file));
		strncat(file, ".jpg", sizeof(file));
		texture = driver->getTexture(file);
	}
	return texture == NULL ? tUnknown : texture;
}
void ImageManager::SetDevice(irr::IrrlichtDevice* dev) {
	device = dev;
	driver = dev->getVideoDriver();
}
void ImageManager::ClearTexture() {
	ScopedLock lk(mutex);
	for(auto tit = tMap.begin(); tit != tMap.end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	for(auto tit = tThumb.begin(); tit != tThumb.end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	tMap.clear();
	tThumb.clear();
}
void ImageManager::RemoveTexture(int code) {
	ScopedLock lk(mutex);
	auto tit = tMap.find(code);
	if(tit != tMap.end()) {
		if(tit->second)
			driver->removeTexture(tit->second);
		tMap.erase(tit);
	}
}
irr::video::ITexture* ImageManager::GetTexture(int code) {
	if(code == 0)
		return tUnknown;
	ScopedLock lk(mutex);
	auto tit = tMap.find(code);
	if(tit == tMap.end()) {
		char file[256];
		sprintf(file, "pics/%d", code);
		irr::video::ITexture* img = LoadTexture(file);
		if(img == NULL) {
			tMap[code] = NULL;
			return tUnknown;
		} else {
			tMap[code] = img;
			return img;
		}
	}
	if(tit->second)
		return tit->second;
	else
		return tUnknown;
}
irr::video::ITexture* ImageManager::GetTextureThumb(int code) {
	if(code == 0)
		return tUnknown;
	ScopedLock lk(mutex);
	auto tit = tThumb.find(code);
	if(tit == tThumb.end()) {
		char file[256];
		sprintf(file, "pics/thumbnail/%d", code);
		irr::video::ITexture* img = LoadTexture(file);
		if(img == NULL) {
			tThumb[code] = NULL;
			return GetTexture(code);
		} else {
			tThumb[code] = img;
			return img;
		}
	}
	if(tit->second)
		return tit->second;
	else
		return GetTexture(code);
}
irr::video::ITexture* ImageManager::GetTextureField(int code) {
	if(code == 0)
		return NULL;
	ScopedLock lk(mutex);
	auto tit = tFields.find(code);
	if(tit == tFields.end()) {
		char file[256];
		sprintf(file, "pics/field/%d", code);
		irr::video::ITexture* img = LoadTexture(file);
		if(img == NULL) {
			tFields[code] = NULL;
			return NULL;
		} else {
			tFields[code] = img;
			return img;
		}
	}
	if(tit->second)
		return tit->second;
	else
		return NULL;
}

void ImageManager::LoadSleeve(int player, wchar_t* site, wchar_t* dir)
{
	TextureData *sleeve = new TextureData;
	sleeve->type = SLEEVE;
	sleeve->player = player;
	std::wcstombs(sleeve->hostname, site, 256);
	std::wcstombs(sleeve->filename, dir, 256);
	if(player == 0)
		std::wcstombs(sleeve->fakename, L"cover0.jpg", 256);
	else
		std::wcstombs(sleeve->fakename, L"cover1.jpg", 256);
	pendingTextures.push_back(sleeve);
}
void ImageManager::LoadPendingTextures()
{
	while (!pendingTextures.empty())
	{
		TextureData *textureData(pendingTextures.back());
		pendingTextures.pop_back();
		ITexture *texture = DownloadTexture(textureData);
		if (texture)
			ApplyTexture(textureData, texture);
		delete textureData;
	}
}
ITexture* ImageManager::DownloadTexture(TextureData *textureData)
{
	sf::Http::Request request(textureData->filename, sf::Http::Request::Get);
	sf::Http http(textureData->hostname);
	sf::Http::Response response = http.sendRequest(request);
	
	if (response.getStatus() == sf::Http::Response::Ok)
	{
		std::string *body = new std::string(response.getBody());
		void *memory = (void *)body->c_str();
		IReadFile *f = device->getFileSystem()->createMemoryReadFile(memory, body->size(), textureData->fakename, false);
		ITexture *texture = driver->getTexture(f);
		return texture;
	}
	return NULL;
}
void ImageManager::ApplyTexture(TextureData *textureData, ITexture *texture)
{
	switch (textureData->type)
	{
	case SLEEVE:
		if (textureData->player >= 0 && textureData->player < 2)
			tCover[textureData->player] = texture;
		break;
	case AVATAR:
		if (textureData->player >= 0 && textureData->player < 4)
			tAvatar[textureData->player] = texture;
		break;
	}
}
}