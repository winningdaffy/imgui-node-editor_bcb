//---------------------------------------------------------------------------

#ifndef xml_interfaceH
#define xml_interfaceH
//---------------------------------------------------------------------------
#include "tinyxml.h"

int get_attribute_data(TiXmlElement *p_element, const char *p_attribute, float *p_data);
int get_attribute_data(TiXmlElement *p_element, const char *p_attribute, double *p_data);
int get_attribute_data(TiXmlElement *p_element, const char *p_attribute, char *p_data, int length);
int get_attribute_data(TiXmlElement *p_element, const char *p_attribute, int *p_data);
int get_attribute_data(TiXmlElement *p_element, const char *p_attribute, unsigned int *p_data);
int get_attribute_data(TiXmlNode *p_node, const char *p_attribute, float *p_data);
int get_attribute_data(TiXmlNode *p_node, const char *p_attribute, double *p_data);
int get_attribute_data(TiXmlNode *p_node, const char *p_attribute, char *p_data, int length);
int get_attribute_data(TiXmlNode *p_node, const char *p_attribute, int *p_data);
int get_attribute_data(TiXmlNode *p_node, const char *p_attribute, unsigned int *p_data);

int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, unsigned int *p_data);
int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, int *p_data);
int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, float *p_data);
int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, char *p_data);
int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, int data);
int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, float data);
int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, double data);
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, unsigned int *p_data);
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, int *p_data);
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, float *p_data);
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, char *p_data);
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, int data);
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, float data);

int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, float *p_data);
int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, double *p_data);
int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, char *p_data, int length);
int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, int *p_data);
int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, unsigned int *p_data);

// template <typename T>
// int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, T *p_data)
//{
//     int rt = false;
//     TiXmlElement *p_element_temp = NULL;
//     p_element_temp = p_node->FirstChildElement(p_data_name);
//     rt = get_attribute_data(p_element_temp, p_attribute, p_data);
//
//     return rt;
// }

#endif
