#pragma once

typedef __int64 INT64;

#define ADD_OFFSET_MANUAL(Klass, Name, Value)	namespace Klass { inline INT64 Name = Value; }
#define ADD_OFFSET(Klass, Name)					ADD_OFFSET_MANUAL(Klass, Name, 0x0)

namespace Offsets {

/* Module Base Offsets */
	ADD_OFFSET(Modbase, World);							// DONE
	ADD_OFFSET(Modbase, Network);						// DONE
	ADD_OFFSET(Modbase, Tick);							// DONE

/* Network Offsets */
	ADD_OFFSET(Network, Scoreboard);

/* PlayerIdentity Offsets */
	ADD_OFFSET(PlayerIdentity, Name);
	ADD_OFFSET(PlayerIdentity, SteamID);
	ADD_OFFSET(PlayerIdentity, NetworkID);

/* World Offsets */										
	ADD_OFFSET(World, BulletList);						// DONE
	ADD_OFFSET(World, ItemList);
	ADD_OFFSET(World, NearEntList);						// DONE
	ADD_OFFSET(World, FarEntList);						// DONE
	ADD_OFFSET(World, Camera);							// DONE
	ADD_OFFSET(World, GrassDensity);
	ADD_OFFSET(World, LocalPlayer);						// DONE
	ADD_OFFSET(World, LocalOffset);	// neg value		// DONE

/* Human Offsets */
	ADD_OFFSET(Human, HumanType);						// DONE
	ADD_OFFSET(Human, VisualState);						// DONE
	ADD_OFFSET(Human, LodShape);						// DONE

/* DayzInfected Offsets */
	ADD_OFFSET(DayZInfected, Skeleton);					// 

/* HumanType Offsets */
	ADD_OFFSET(HumanType, ObjectName);					// DONE
	ADD_OFFSET(HumanType, CategoryName);				// DONE
	ADD_OFFSET(HumanType, FullName);


/* DayZLocal Offsets */
	/* FIGURE OUT A WAY TO GET ENFUSION PTR - COULD BE FUCKING COOOOOOOL */

/* DayZPlayer Offsets */
	ADD_OFFSET(DayZPlayer, Skeleton);					// DONE
	ADD_OFFSET(DayZPlayer, NetworkID);					// DONE
	ADD_OFFSET(DayZPlayer, Inventory);					// DONE

/* DayZPlayerInventory Offsets */
	ADD_OFFSET(DayZPlayerInventory, Hands);				// DONE

/* InventoryItem Offsets */
	ADD_OFFSET(InventoryItem, ItemInventory);			// DONE

/* Weapon Offsets */									
	ADD_OFFSET(Weapon, WeaponIndex);					// DONE
	ADD_OFFSET(Weapon, WeaponInfoTable);				// DONE
	ADD_OFFSET(Weapon, MuzzleCount);					// DONE
	ADD_OFFSET(Weapon, WeaponInfoSize);					// DONE

/* WeaponInventory Offsets */
	ADD_OFFSET(WeaponInventory, MagazineRef);			// DONE

/* Magazine Offsets */
	ADD_OFFSET(Magazine, MagazineType);					// DONE
	ADD_OFFSET(Magazine, AmmoCount);					// DONE

/* AmmoType Offsets */
	ADD_OFFSET(AmmoType, InitSpeed);					// DONE
	ADD_OFFSET(AmmoType, AirFriction);					// DONE

/* Skeleton Offsets */
	ADD_OFFSET(Skeleton, AnimClass1);					// DONE
	ADD_OFFSET(Skeleton, AnimClass2);					// DONE

/* AnimClass Offsets */
	ADD_OFFSET(AnimClass, MatrixArray);					// DONE
	ADD_OFFSET_MANUAL(AnimClass, MatrixEntry, 0x54);	// DONE

/* Camera Offsets */
	ADD_OFFSET(Camera, ViewMatrix);						// DONE
	ADD_OFFSET(Camera, ViewPortMatrix);					// DONE
	ADD_OFFSET(Camera, ViewProjection);					// DONE - ViewProjectionD2 after this + sizeof(Vector3);
														
/* VisualState Offsets */
	ADD_OFFSET(VisualState, Transform);					// DONE
	ADD_OFFSET(VisualState, InverseTransform);			// DONE
	ADD_OFFSET(VisualState, Velocity);					// -> Cannot find :/
}