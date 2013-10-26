#include "image_manager.h"
#include <SFML/Network.hpp>

namespace ygo {

ImageManager imageManager;

bool ImageManager::Initial() {
	tCover = driver->getTexture("textures/cover.jpg");
	tCover2 = driver->getTexture("textures/cover2.jpg");
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
	tField = driver->getTexture("textures/field.png");
	tFieldTransparent = driver->getTexture("textures/field-transparent.png");
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
	SleeveData *sleeve = new SleeveData;
	sleeve->player = player;
	std::wcstombs(sleeve->siteurl, site, 256);
	std::wcstombs(sleeve->sitedir, dir, 256);
	pendingSleeves.push_back(sleeve);
}
void ImageManager::LoadPendingSleeves()
{
	while (!pendingSleeves.empty())
	{
		SleeveData *sleeve(pendingSleeves.back());
		pendingSleeves.pop_back();

		sf::Http::Request request(sleeve->sitedir, sf::Http::Request::Get);
		sf::Http http(sleeve->siteurl);
		sf::Http::Response response = http.sendRequest(request);

		if (response.getStatus() == sf::Http::Response::Ok)
		{
			std::string *body = new std::string(response.getBody());
			void *memory = (void *)body->c_str();
			char *fakename = sleeve->player == 0 ? "cover0.jpg" : "cover1.jpg";
			IReadFile *f = device->getFileSystem()->createMemoryReadFile(memory, body->size(), fakename, false);
			ITexture *texture = driver->getTexture(f);
			if (texture)
			{
				if (sleeve->player == 0)
					tCover = texture;
				else
					tCover2 = texture;
			}
		}
		delete sleeve;
	}
}
}