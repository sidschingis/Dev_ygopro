#include "image_manager.h"
#include <SFML/Network.hpp>

namespace ygo {

ImageManager imageManager;

bool ImageManager::Initial() {
	tCover[0] = driver->getTexture("textures/cover.jpg");
	tCover[1] = driver->getTexture("textures/cover2.jpg");
	tUnknown = driver->getTexture("textures/unknown.jpg");
	tAct = driver->getTexture("textures/act.png");
	tAttack = driver->getTexture("textures/attack.png");
	tChain = driver->getTexture("textures/chain.png");
	tNegated = driver->getTexture("textures/negated.png");
	tNumber = driver->getTexture("textures/number.png");
	tLPBar = driver->getTexture("textures/lp.png");
	tLPFrame = driver->getTexture("textures/lpf.png");
	tMask = driver->getTexture("textures/mask.png");
	tEquip = driver->getTexture("textures/equip.png");
	tTarget = driver->getTexture("textures/target.png");
	tLim = driver->getTexture("textures/lim.png");
	tHand[0] = driver->getTexture("textures/f1.jpg");
	tHand[1] = driver->getTexture("textures/f2.jpg");
	tHand[2] = driver->getTexture("textures/f3.jpg");
	tBackGround = driver->getTexture("textures/bg.jpg");
	tBackGround2 = driver->getTexture("textures/bg2.jpg");
	tBackGround_menu = driver->getTexture("textures/bg_menu.jpg");
	tBackGround_deck = driver->getTexture("textures/bg_deck.jpg");
	tField = driver->getTexture("textures/field2.png");
	tFieldTransparent = driver->getTexture("textures/field-transparent.png");
	for (int i = 0; i < 4; ++i)
		tAvatar[i] = NULL;
	return true;
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
		sprintf(file, "pics/%d.jpg", code);
		irr::video::ITexture* img = driver->getTexture(file);
		if(img == NULL) {
			return GetTextureThumb(code);
		} else {
			tMap[code] = img;
			return img;
		}
	}
	if(tit->second)
		return tit->second;
	else
		return GetTextureThumb(code);
}
irr::video::ITexture* ImageManager::GetTextureThumb(int code) {
	if(code == 0)
		return tUnknown;
	ScopedLock lk(mutex);
	auto tit = tThumb.find(code);
	if(tit == tThumb.end()) {
		char file[256];
		sprintf(file, "pics/thumbnail/%d.jpg", code);
		irr::video::ITexture* img = driver->getTexture(file);
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
		sprintf(file, "pics/field/%d.png", code);
		irr::video::ITexture* img = driver->getTexture(file);
		if(img == NULL) {
			printf(file, "pics/field/%d.jpg", code);
			img = driver->getTexture(file);
			if (img == NULL) {
				tFields[code] = NULL;
				return NULL;
			}
			else {
				tFields[code] = img;
				return img;
			}
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

irr::video::ITexture* ImageManager::GetFieldTexture(int code, int player) {
	if(code == 0)
		return NULL;
	ScopedLock lk(mutex);
	int fieldcode = code + player;
	auto tit = tSpellFields.find(fieldcode);
	if(tit == tSpellFields.end()) {
		irr::video::ITexture* rt = 0;
		rt = driver->addRenderTargetTexture(core::dimension2d<u32>(512,512));
		driver->setRenderTarget(rt, false, false, video::SColor(0,0,0,255));
		ITexture *texture = imageManager.GetTextureField(code);
		if(texture)
			driver->draw2DImage(texture, irr::core::rect<s32>(0,0,512,512),player == 0 ? irr::core::rect<s32>(0,256,512,512) : irr::core::rect<s32>(0,0,512,256));
		driver->setRenderTarget(0, false, false, 0);
		tSpellFields[fieldcode] = rt;
		return rt;
	}

	if(tit->second)
		return tit->second;
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