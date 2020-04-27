# Import all necessary modules from Maya's commands API.
from maya.cmds import getAttr, setAttr, listRelatives, polyEvaluate, pointPosition;

def set_data_string(procedural_node_shape_name):
	"""
	This sets the string of the data attribute of the RenderMan procedural node that is then passed into the procedural plugin.

 	Parameters
	----------
	procedural_node_shape_name : str
		The name of the RenderMan procedural node's shape.
	"""
	
	# Each of the extra attributes added to the RenderMan procedural node's shape are copied as strings.
	size_variance = get_attribute_as_string(procedural_node_shape_name, "size_variance");
	spacing_variance = get_attribute_as_string(procedural_node_shape_name, "spacing_variance");
	rotation_variance = get_attribute_as_string(procedural_node_shape_name, "rotation_variance");
	ice_path = get_attribute_as_string(procedural_node_shape_name, "ice_path");
	initial_scale = get_attribute_as_string(procedural_node_shape_name, "initial_scale");
	
	# The coordinates for where the ice can be placed are fetched then the total number of them as well as the coordinates themselves are converted to strings. 
	surface_coordinates = get_whiskey_surface_coordinates(get_attribute_as_string(procedural_node_shape_name, "whiskey_shape_name"));
	coordinate_count = str(len(surface_coordinates));
	surface_coordinates_string = " ".join(map(str, surface_coordinates));
		
	# All the previous data is then concatenated together into a string that is then set to the data attribute.
	data_string = create_data_string(size_variance, spacing_variance, rotation_variance, ice_path, initial_scale,coordinate_count, surface_coordinates_string);
	setAttr(procedural_node_shape_name + ".data", data_string, type="string");

def get_attribute_as_string(procedural_node_shape_name, attribute_name):
	"""
	This gets the data from the RenderMan procedural node's shape then converts it to a string.

 	Parameters
	----------
	procedural_node_shape_name : str
		The name of the RenderMan procedural node's shape.
	attribute_name : str
		The name of the RenderMan procedural node's shape extra attribute.
		
	Returns
	-------
	str
		The extra attribute from the RenderMan procedural node's shape as a string.
	"""
	
	return str(getAttr(procedural_node_shape_name + '.' + attribute_name));
	
def create_data_string(*attributes):
	"""
	This concatenates the data strings together to form the singular string that is applied to the data attribute.

 	Parameters
	----------
	*attributes : list
		The list of the data strings to be concatenated together.
		
	Returns
	-------
	str
		The concatenated data string.
	"""
	
	data_string = "";
	first_string = True;
	
	for attribute in attributes:
		
		# If this is the first string to be concatenated, don't add a space to the front of it.
		if (first_string):
			data_string += attribute;
			first_string = False;
		else:
			data_string += " " + attribute;
	
	return data_string;
	
def get_whiskey_surface_coordinates(shape_name):
	"""
	This gets the coordinates the ice can be created on from the whiskey mesh.
	
	Parameters
	----------
	shape_name : str
		The name of the whiskey mesh.
	
	Returns
	-------
	list
		A list of all the vertices where an ice cube can be place.
	"""

	vertices = [];
	shape = listRelatives(shape_name, shapes=True)[0];
	for current_vertex in range(142, 61, -1):
		vertex_string = shape + '.vtx[%d]' % current_vertex;
		vertex_position = pointPosition(vertex_string); 
		vertices.extend(vertex_position);
	return vertices;
