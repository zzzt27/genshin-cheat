#pragma once

#include "AdvancedFilter.h"
#include "SimpleFilter.h"
#include "ChestFilter.h"

namespace cheat::game::filters
{
	namespace collection
	{
		extern SimpleFilter Book;
		extern SimpleFilter Viewpoint;

		extern ChestFilter RadiantSpincrystal;
		extern ChestFilter BookPage;
		extern ChestFilter QuestInteract;
	}

	namespace chest
	{
		extern ChestFilter CommonChest;
		extern ChestFilter ExquisiteChest;
		extern ChestFilter PreciousChest;
		extern ChestFilter LuxuriousChest;
		extern ChestFilter RemarkableChest;

		extern ChestFilter SLocked;
		extern ChestFilter SInLock;
		extern ChestFilter SFrozen;
		extern ChestFilter SBramble;
		extern ChestFilter STrap;
	}

	namespace featured
	{
		extern SimpleFilter Anemoculus;
		extern SimpleFilter CrimsonAgate;
		extern SimpleFilter Electroculus;
		extern SimpleFilter Electrogranum;
		extern SimpleFilter FishingPoint;
		extern SimpleFilter Geoculus;
		extern SimpleFilter Lumenspar;
		extern SimpleFilter KeySigil;
		extern SimpleFilter ShrineOfDepth;
		extern SimpleFilter TimeTrialChallenge;
	}

	namespace guide
	{
		extern SimpleFilter CampfireTorch;
		extern SimpleFilter MysteriousCarvings;
		extern SimpleFilter PhaseGate;
		extern SimpleFilter Pot;
		extern SimpleFilter RuinBrazier;
		extern SimpleFilter Stormstone;
	}

	namespace living
	{
		extern SimpleFilter BirdEgg;
		extern SimpleFilter ButterflyWings;
		extern SimpleFilter Crab;
		extern SimpleFilter CrystalCore;
		extern SimpleFilter Fish;
		extern SimpleFilter Frog;
		extern SimpleFilter LizardTail;
		extern SimpleFilter LuminescentSpine;
		extern SimpleFilter Onikabuto;
		extern SimpleFilter Starconch;
		extern SimpleFilter UnagiMeat;
	}

	namespace mineral
	{
		extern SimpleFilter AmethystLump;
		extern SimpleFilter ArchaicStone;
		extern SimpleFilter CorLapis;
		extern SimpleFilter CrystalChunk;
		extern SimpleFilter CrystalMarrow;
		extern SimpleFilter ElectroCrystal;
		extern SimpleFilter IronChunk;
		extern SimpleFilter NoctilucousJade;
		extern SimpleFilter MagicalCrystalChunk;
		extern SimpleFilter StarSilver;
		extern SimpleFilter WhiteIronChunk;
	}

	namespace monster
	{
		extern SimpleFilter AbyssMage;
		extern SimpleFilter FatuiAgent;
		extern SimpleFilter FatuiCicinMage;
		extern SimpleFilter FatuiMirrorMaiden;
		extern SimpleFilter FatuiSkirmisher;
		extern SimpleFilter Geovishap;
		extern SimpleFilter GeovishapHatchling;
		extern SimpleFilter Hilichurl;
		extern SimpleFilter Mitachurl;
		extern SimpleFilter Nobushi;
		extern SimpleFilter RuinGuard;
		extern SimpleFilter RuinHunter;
		extern SimpleFilter RuinSentinel;
		extern SimpleFilter Samachurl;
		extern SimpleFilter ShadowyHusk;
		extern SimpleFilter Slime;
		extern SimpleFilter FloatingFungus;
		extern SimpleFilter Specter;
		extern SimpleFilter TreasureHoarder;
		extern SimpleFilter UnusualHilichurl;
		extern SimpleFilter Whopperflower;
		extern SimpleFilter WolvesOfTheRift;
	}

	namespace plant
	{
		extern SimpleFilter AmakumoFruit;
		extern SimpleFilter Apple;
		extern SimpleFilter BambooShoot;
		extern SimpleFilter Berry;
		extern SimpleFilter CallaLily;
		extern SimpleFilter Carrot;
		extern SimpleFilter Cecilia;
		extern SimpleFilter DandelionSeed;
		extern SimpleFilter Dendrobium;
		extern SimpleFilter FlamingFlowerStamen;
		extern SimpleFilter FluorescentFungus;
		extern SimpleFilter GlazeLily;
		extern SimpleFilter Horsetail;
		extern SimpleFilter JueyunChili;
		extern SimpleFilter LavenderMelon;
		extern SimpleFilter LotusHead;
		extern SimpleFilter Matsutake;
		extern SimpleFilter Mint;
		extern SimpleFilter MistFlowerCorolla;
		extern SimpleFilter Mushroom;
		extern SimpleFilter NakuWeed;
		extern SimpleFilter PhilanemoMushroom;
		extern SimpleFilter Pinecone;
		extern SimpleFilter Qingxin;
		extern SimpleFilter Radish;
		extern SimpleFilter SakuraBloom;
		extern SimpleFilter SangoPearl;
		extern SimpleFilter SeaGanoderma;
		extern SimpleFilter Seagrass;
		extern SimpleFilter SilkFlower;
		extern SimpleFilter SmallLampGrass;
		extern SimpleFilter Snapdragon;
		extern SimpleFilter Sunsettia;
		extern SimpleFilter SweetFlower;
		extern SimpleFilter Valberry;
		extern SimpleFilter Violetgrass;
		extern SimpleFilter WindwheelAster;
		extern SimpleFilter Wolfhook;
	}

	namespace puzzle
	{
		extern SimpleFilter AncientRime;
		extern SimpleFilter BakeDanuki;
		extern SimpleFilter BloattyFloatty;
		extern AdvancedFilter CubeDevices;

		extern SimpleFilter EightStoneTablets;
		extern SimpleFilter ElectricConduction;
		extern AdvancedFilter ElectroSeelie;
		extern SimpleFilter ElementalMonument;
		extern SimpleFilter FloatingAnemoSlime;
		extern SimpleFilter Geogranum;
		extern SimpleFilter GeoPuzzle;
		extern SimpleFilter LargeRockPile;
		extern SimpleFilter LightUpTilePuzzle;
		extern SimpleFilter LightningStrikeProbe;
		extern SimpleFilter LuminousSeelie;
		extern SimpleFilter MistBubble;
		extern SimpleFilter PirateHelm;
		extern SimpleFilter PressurePlate;
		extern SimpleFilter SeelieLamp;
		extern SimpleFilter Seelie;
		extern SimpleFilter SmallRockPile;
		extern SimpleFilter StormBarrier;
		extern SimpleFilter SwordHilt;
		extern SimpleFilter TorchPuzzle;
		extern SimpleFilter UniqueRocks;
		extern SimpleFilter WarmingSeelie;
		extern SimpleFilter WindmillMechanism;
	}

	namespace combined
	{
		extern SimpleFilter Oculies;
		extern SimpleFilter Chests;
		extern SimpleFilter Monsters;
		extern AdvancedFilter Animals;
	}
}