from maya.cmds import getAttr, setAttr, listRelatives, polyEvaluate, pointPosition;

def set_data_string(procedural_node_shape_name):
	size_variance = get_attribute_as_string(procedural_node_shape_name, "size_variance");
	spacing_variance = get_attribute_as_string(procedural_node_shape_name, "spacing_variance");
	rotation_variance = get_attribute_as_string(procedural_node_shape_name, "rotation_variance");
	ice_path = get_attribute_as_string(procedural_node_shape_name, "ice_path");
	initial_scale = get_attribute_as_string(procedural_node_shape_name, "initial_scale");
	surface_coordinates = get_whiskey_surface_coordinates();
	coordinate_count = str(len(surface_coordinates));
	surface_coordinates_string = " ".join(map(str, surface_coordinates));
		
	
	data_string = create_data_string(size_variance, spacing_variance, rotation_variance, ice_path, initial_scale,coordinate_count, surface_coordinates_string);
	setAttr(procedural_node_shape_name + ".data", data_string, type="string");

def get_attribute_as_string(procedural_node_shape_name, attribute_name):
	return str(getAttr(procedural_node_shape_name + '.' + attribute_name));
	
def create_data_string(*attributes):
	data_string = "";
	first_string = True;
	for attribute in attributes:
		if (first_string):
			data_string += attribute;
			first_string = False;
		else:
			data_string += " " + attribute;
	return data_string;
	
def get_whiskey_surface_coordinates():

	shape_name = "whiskey";
	vertices = [];
	shape = listRelatives(shape_name, shapes=True)[0];
	for current_vertex in range(142, 61, -1):
		vertex_string = shape + '.vtx[%d]' % current_vertex;
		vertex_position = pointPosition(vertex_string); 
		vertices.extend(vertex_position);
	return vertices;
