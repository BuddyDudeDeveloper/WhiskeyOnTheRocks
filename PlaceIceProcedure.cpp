#include <ri.h>
#include <RixInterfaces.h>
#include <RiTypesHelper.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
//#include "getFiles.h"

// A RiProcedural must implement these functions. This is a fixed requirement.
extern "C"
{
	PRMANEXPORT RtPointer ConvertParameters ( RtString paramStr              );
	PRMANEXPORT RtVoid    Subdivide         ( RtPointer data, RtFloat detail );
	PRMANEXPORT RtVoid    Free              ( RtPointer data                 );
}

typedef struct {

	RtFloat size_variance;
	RtFloat spacing_variance;
	RtFloat rotation_variance;
	std::string ice_path;
	RtFloat initial_scale;
	RtInt coordinate_count;
	RtFloat *surface_coordinates;
} IceData;

RtFloat randBetween(RtFloat min, RtFloat max);
RtBoolean isOverlapping(RtFloat x, RtFloat y, RtFloat z, RtFloat other_x, RtFloat other_y, RtFloat other_z);
std::vector<std::string> winGetFiles(std::string pattern);
std::vector<std::string> globGetFiles(const std::string& pattern);
std::vector<std::string> getFiles(std::string pattern);

RtPointer ConvertParameters(RtString paramStr) {
	// The strtok() function cannot be used on the paramStr directly because
	// it modifies the string. 
	long len = strlen(paramStr);
	
	// We could directly create a copy of the input paramStr as an array and
	// use the strcpy(), string copy, function.
	//char copyStr[len];
	//strcpy(copyStr, paramStr);
	
	// However, because the paramStr can be very large we allocate memory
	// from the main memory pool (the "heap") and then perform a block 
	// copy of the contents of paramStr.
	char *copyStr = (char*)calloc(len + 1, sizeof(char));
	memcpy(copyStr, paramStr, len + 1);
	
	// Allocate a block of memory to store one instance of SpheresData.
	IceData *dataPtr = (IceData*)calloc(1, sizeof(IceData));
	
	// Irrespective of how many values are specified by the paramStr we
	// know the first two values will specify the radius of the spheres
	// and the number of coordinates that define their 3D locations.
	
	char path_characters[512];
	
	sscanf(copyStr, "%f %f %f %s %f %d", &dataPtr->size_variance, &dataPtr->spacing_variance, &dataPtr->rotation_variance, path_characters, &dataPtr->initial_scale, &dataPtr->coordinate_count);
	
	std::string path(path_characters);
	
	std::vector<std::string> paths = getFiles(path);
	dataPtr->ice_path = paths[0];
	
	// Allocate memory to store an array of coordinates
	RtInt coordinate_count = dataPtr->coordinate_count;
	dataPtr->surface_coordinates = (RtFloat*)calloc(coordinate_count, sizeof(RtFloat));
	
	char *strPtr = strtok(copyStr, " ");
	
	for(int deleted_values = 0; deleted_values < 6; deleted_values++) {
		strPtr = strtok(NULL, " ");
	}
	long current_index = 0;
	while(strPtr) {
		// Convert each string to a double precision floating point number
		dataPtr->surface_coordinates[current_index] = strtod(strPtr, NULL);
		current_index++;
		strPtr = strtok(NULL, " "); // grab the next part of copyStr.
		}
	// Don't forget to free the memory that was allocated for the copied text.
	free(copyStr);
	return (RtPointer)dataPtr;
}


// ----------------------------------------------------
// A RiProcedural required function
// ----------------------------------------------------
RtVoid Subdivide(RtPointer data, RtFloat detail) {
	
    RtFloat size_variance = ((IceData*)data)->size_variance;
 	RtFloat spacing_variance = ((IceData*)data)->spacing_variance;
	RtFloat rotation_variance = ((IceData*)data)->rotation_variance;
	std::string ice_path = ((IceData*)data)->ice_path;
	RtInt coordinate_count = ((IceData*)data)->coordinate_count;
	RtFloat initial_scale = ((IceData*)data)->initial_scale;
	RtFloat *surface_coordinates =  ((IceData*)data)->surface_coordinates;
	std::vector <RtFloat> other_coordinates;

	for(int current_index = 0; current_index < coordinate_count; current_index += 3) {
			
			RtFloat x = surface_coordinates[current_index] + randBetween(-spacing_variance, spacing_variance);
			RtFloat y = surface_coordinates[current_index + 1];
			RtFloat z = surface_coordinates[current_index + 2] + randBetween(-spacing_variance, spacing_variance);
			
			
			RtBoolean overlaps_another = false;
			for(int current_other_index = 0; current_other_index < other_coordinates.size(); current_other_index += 3) {
				if(overlaps_another) {
					break;
				}
				RtFloat other_x = other_coordinates[current_other_index];
				RtFloat other_y = other_coordinates[current_other_index + 1];
				RtFloat other_z = other_coordinates[current_other_index + 2];
				overlaps_another = isOverlapping(x, y, z, other_x, other_y, other_z);
			} 
			
			if(other_coordinates.size() == 0 || !overlaps_another) {
					
					printf("x: %f\n", x);
					RiTransformBegin();
					RiTranslate(x,y,z);
					RiRotate(randBetween(-rotation_variance, rotation_variance), randBetween(0, 1), randBetween(0, 1), randBetween(0, 1));
					RiScale(initial_scale + randBetween(0,size_variance), initial_scale + randBetween(0,size_variance), initial_scale + randBetween(0,size_variance));
					RiReadArchiveV(ice_path.c_str(), NULL, 0, NULL, NULL);
		
					RiTransformEnd();
				
					other_coordinates.push_back(x);
					other_coordinates.push_back(y);
					other_coordinates.push_back(z);
			
			}
			
			
			
	}
}
// ----------------------------------------------------
// A RiProcedural required function
// ----------------------------------------------------
RtVoid Free(RtPointer data) {
	free(((IceData*)data)->surface_coordinates);
    free(data);
}
	
RtBoolean isOverlapping(RtFloat x, RtFloat y, RtFloat z, RtFloat other_x, RtFloat other_y, RtFloat other_z) {
	
	RtFloat distance_to_edge = .25;
	
	RtFloat minX = x - distance_to_edge;
	RtFloat minY = y - distance_to_edge;
	RtFloat minZ = z - distance_to_edge;
	
	RtFloat otherMinX = other_x - distance_to_edge;
	RtFloat otherMinY = other_y - distance_to_edge;
	RtFloat otherMinZ = other_z - distance_to_edge;
	
	RtFloat maxX = x + distance_to_edge;
	RtFloat maxY = y + distance_to_edge;
	RtFloat maxZ = z + distance_to_edge;
	
	RtFloat otherMaxX = other_x + distance_to_edge;
	RtFloat otherMaxY = other_y + distance_to_edge;
	RtFloat otherMaxZ = other_z + distance_to_edge;
	
	RtBoolean xOverlaps = (minX <= otherMaxX && maxX >= otherMinX);
	RtBoolean yOverlaps = (minY <= otherMaxY && maxY >= otherMinY);
	RtBoolean zOverlaps = (minZ <= otherMaxZ && maxZ >= otherMinZ);
	
	return xOverlaps && yOverlaps && zOverlaps;
}

RtFloat randBetween(RtFloat min, RtFloat max) {
    return ((RtFloat)rand()/RAND_MAX) * (max - min) + min;
}

#ifdef  _WIN32
	#include <Windows.h>
	std::vector<std::string> winGetFiles(std::string pattern) {
	    std::vector<std::string> paths;
		std::string parentDir = pattern.substr(0, pattern.find_last_of("/\\"));
		//printf("%s\n", parentDir.c_str());
	    WIN32_FIND_DATA fd;
	    HANDLE hFind = ::FindFirstFile(pattern.c_str(), &fd);
	    if(hFind != INVALID_HANDLE_VALUE) {
	        do {
	            // read all (real) files in current folder
	            // , delete '!' read other 2 default folder . and ..
	            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
	                paths.push_back(parentDir + "/" + fd.cFileName);
	            }
	        } while(::FindNextFile(hFind, &fd));
	        ::FindClose(hFind);
	    }
	    return paths;
	}
#else
	#include <glob.h>
	std::vector<std::string> globGetFiles(const std::string& pattern){
	    glob_t glob_result;
	    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
	    std::vector<std::string> paths;
	    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
	        paths.push_back(std::string(glob_result.gl_pathv[i]));
	    	}
	    globfree(&glob_result);
	    return paths;
		}
#endif

std::vector<std::string> getFiles(std::string pattern) {
	#ifdef  _WIN32
		return winGetFiles(pattern);
	#else
		return globGetFiles(pattern);
	#endif
	}
