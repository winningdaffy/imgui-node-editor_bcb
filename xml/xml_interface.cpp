//---------------------------------------------------------------------------

#pragma hdrstop

#include <string>
#include "xml_interface.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

int get_attribute_data(TiXmlElement *p_element, const char *p_attribute, float *p_data)
{
    int rt = false;
    int ret = TIXML_SUCCESS;
    if (p_element != NULL)
    {
        ret = p_element->QueryFloatAttribute(p_attribute, p_data);
        if (ret == TIXML_SUCCESS)
        {
            rt = true;
        }
        else
        {
            // TODO add log
            if (ret == TIXML_WRONG_TYPE)
            {
            }
            else if (ret == TIXML_NO_ATTRIBUTE)
            {
            }
            rt = false;
        }
    }
    return rt;
}

int get_attribute_data(TiXmlElement *p_element, const char *p_attribute, double *p_data)
{
    int rt = false;
    int ret = TIXML_SUCCESS;
    if (p_element != NULL)
    {
        ret = p_element->QueryDoubleAttribute(p_attribute, p_data);
        if (ret == TIXML_SUCCESS)
        {
            rt = true;
        }
        else
        {
            // TODO add log
            if (ret == TIXML_WRONG_TYPE)
            {
            }
            else if (ret == TIXML_NO_ATTRIBUTE)
            {
            }
            rt = false;
        }
    }
    return rt;
}

int get_attribute_data(TiXmlElement *p_element, const char *p_attribute, char *p_data, int length)
{
    int rt = false;
    int ret = TIXML_SUCCESS;
    if (p_element != NULL)
    {
        std::string temp_str;
        ret = p_element->QueryStringAttribute(p_attribute, &temp_str);
        if (ret == TIXML_SUCCESS)
        {
            strcpy_s(p_data, length, temp_str.c_str());
            rt = true;
        }
        else
        {
            // TODO add log
            if (ret == TIXML_WRONG_TYPE)
            {
            }
            else if (ret == TIXML_NO_ATTRIBUTE)
            {
            }
            rt = false;
        }
    }
    return rt;
}

int get_attribute_data(TiXmlElement *p_element, const char *p_attribute, int *p_data)
{
    int rt = false;
    int ret = TIXML_SUCCESS;
    if (p_element != NULL)
    {
        ret = p_element->QueryIntAttribute(p_attribute, p_data);
        if (ret == TIXML_SUCCESS)
        {
            rt = true;
        }
        else
        {
            // TODO add log
            if (ret == TIXML_WRONG_TYPE)
            {
            }
            else if (ret == TIXML_NO_ATTRIBUTE)
            {
            }
            rt = false;
        }
    }
    return rt;
}

int get_attribute_data(TiXmlElement *p_element, const char *p_attribute, unsigned int *p_data)
{
    int rt = false;
    int ret = TIXML_SUCCESS;
    if (p_element != NULL)
    {
        ret = p_element->QueryUnsignedAttribute(p_attribute, p_data);
        if (ret == TIXML_SUCCESS)
        {
            rt = true;
        }
        else
        {
            // TODO add log
            if (ret == TIXML_WRONG_TYPE)
            {
            }
            else if (ret == TIXML_NO_ATTRIBUTE)
            {
            }
            rt = false;
        }
    }
    return rt;
}

int get_attribute_data(TiXmlNode *p_node, const char *p_attribute, float *p_data)
{
    int ret = false;
    if (p_node != NULL)
    {
        TiXmlElement *p_element = p_node->ToElement();
        ret = get_attribute_data(p_element, p_attribute, p_data);
    }
    return ret;
}
int get_attribute_data(TiXmlNode *p_node, const char *p_attribute, double *p_data)
{
    int ret = false;
    if (p_node != NULL)
    {
        TiXmlElement *p_element = p_node->ToElement();
        ret = get_attribute_data(p_element, p_attribute, p_data);
    }
    return ret;
}
int get_attribute_data(TiXmlNode *p_node, const char *p_attribute, char *p_data, int length)
{
    int ret = false;
    if (p_node != NULL)
    {
        TiXmlElement *p_element = p_node->ToElement();
        ret = get_attribute_data(p_element, p_attribute, p_data, length);
    }
    return ret;
}
int get_attribute_data(TiXmlNode *p_node, const char *p_attribute, int *p_data)
{
    int ret = false;
    if (p_node != NULL)
    {
        TiXmlElement *p_element = p_node->ToElement();
        ret = get_attribute_data(p_element, p_attribute, p_data);
    }
    return ret;
}
int get_attribute_data(TiXmlNode *p_node, const char *p_attribute, unsigned int *p_data)
{
    int ret = false;
    if (p_node != NULL)
    {
        TiXmlElement *p_element = p_node->ToElement();
        ret = get_attribute_data(p_element, p_attribute, p_data);
    }
    return ret;
}

int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, unsigned int *p_data)
{
    TiXmlElement *p_element_temp;
    int rt = false;
    if (p_element != NULL &&
        p_data != NULL)
    {
        p_element_temp = new TiXmlElement(p_data_name);
        p_element_temp->SetAttribute(p_attribute, *p_data);
        p_element->LinkEndChild(p_element_temp);
        rt = true;
    }
    return rt;
}

int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, int *p_data)
{
    TiXmlElement *p_element_temp;
    int rt = false;
    if (p_element != NULL &&
        p_data != NULL)
    {
        p_element_temp = new TiXmlElement(p_data_name);
        p_element_temp->SetAttribute(p_attribute, *p_data);
        p_element->LinkEndChild(p_element_temp);
        rt = true;
    }
    return rt;
}

int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, float *p_data)
{
    TiXmlElement *p_element_temp;
    int rt = false;
    char value_str[64];
    if (p_element != NULL &&
        p_data != NULL)
    {
        p_element_temp = new TiXmlElement(p_data_name);
        snprintf(value_str, 50, "%.15f", *p_data);
        p_element_temp->SetAttribute(p_attribute, value_str);
        p_element->LinkEndChild(p_element_temp);
        rt = true;
    }
    return rt;
}

int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, char *p_data)
{
    TiXmlElement *p_element_temp;
    int rt = false;
    if (p_element != NULL &&
        p_data != NULL)
    {
        p_element_temp = new TiXmlElement(p_data_name);
        p_element_temp->SetAttribute(p_attribute, p_data);
        p_element->LinkEndChild(p_element_temp);
        rt = true;
    }
    return rt;
}

int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, int data)
{
    TiXmlElement *p_element_temp;
    int rt = false;
    if (p_element != NULL)
    {
        p_element_temp = new TiXmlElement(p_data_name);
        p_element_temp->SetAttribute(p_attribute, data);
        p_element->LinkEndChild(p_element_temp);
        rt = true;
    }
    return rt;
}

int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, float data)
{
    TiXmlElement *p_element_temp;
    int rt = false;
    char value_str[64];
    if (p_element != NULL)
    {
        p_element_temp = new TiXmlElement(p_data_name);
        snprintf(value_str, 50, "%.15f", data);
        p_element_temp->SetAttribute(p_attribute, value_str);
        p_element->LinkEndChild(p_element_temp);
        rt = true;
    }
    return rt;
}

int set_attribute_data(TiXmlElement *p_element, const char *p_data_name, const char *p_attribute, double data)
{
    TiXmlElement *p_element_temp;
    int rt = false;
    char value_str[64];
    if (p_element != NULL)
    {
        p_element_temp = new TiXmlElement(p_data_name);
        snprintf(value_str, 50, "%.15f", data);
        p_element_temp->SetAttribute(p_attribute, value_str);
        p_element->LinkEndChild(p_element_temp);
        rt = true;
    }
    return rt;
}

int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, unsigned int *p_data)
{
    int rt = false;
    if (p_element != NULL &&
        p_data != NULL)
    {
        p_element->SetAttribute(p_attribute, *p_data);
        rt = true;
    }
    return rt;
}
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, int *p_data)
{
    int rt = false;
    if (p_element != NULL &&
        p_data != NULL)
    {
        p_element->SetAttribute(p_attribute, *p_data);
        rt = true;
    }
    return rt;
}
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, float *p_data)
{
    char value_str[64];
    int rt = false;
    if (p_element != NULL &&
        p_data != NULL)
    {
        snprintf(value_str, 50, "%.15f", *p_data);
        p_element->SetAttribute(p_attribute, value_str);
        rt = true;
    }
    return rt;
}
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, char *p_data)
{
    int rt = false;
    if (p_element != NULL &&
        p_data != NULL)
    {
        p_element->SetAttribute(p_attribute, p_data);
        rt = true;
    }
    return rt;
}
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, int data)
{
    int rt = false;
    if (p_element != NULL)
    {
        p_element->SetAttribute(p_attribute, data);
        rt = true;
    }
    return rt;
}
int set_attribute_data(TiXmlElement *p_element, const char *p_attribute, float data)
{
    char value_str[64];
    int rt = false;
    if (p_element != NULL)
    {
        snprintf(value_str, 50, "%.15f", data);
        p_element->SetAttribute(p_attribute, value_str);
        rt = true;
    }
    return rt;
}

int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, float *p_data)
{
    int rt = false;
    TiXmlElement *p_element_temp = NULL;
    if (p_node != NULL)
    {
        p_element_temp = p_node->FirstChildElement(p_data_name);
        rt = get_attribute_data(p_element_temp, p_attribute, p_data);
    }

    return rt;
}

int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, double *p_data)
{
    int rt = false;
    TiXmlElement *p_element_temp = NULL;
    if (p_node != NULL)
    {
        p_element_temp = p_node->FirstChildElement(p_data_name);
        rt = get_attribute_data(p_element_temp, p_attribute, p_data);
    }

    return rt;
}

int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, char *p_data, int length)
{
    int rt = false;
    TiXmlElement *p_element_temp = NULL;
    if (p_node != NULL)
    {
        p_element_temp = p_node->FirstChildElement(p_data_name);
        rt = get_attribute_data(p_element_temp, p_attribute, p_data, length);
    }

    return rt;
}

int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, int *p_data)
{
    int rt = false;
    TiXmlElement *p_element_temp = NULL;
    if (p_node != NULL)
    {
        p_element_temp = p_node->FirstChildElement(p_data_name);
        rt = get_attribute_data(p_element_temp, p_attribute, p_data);
    }

    return rt;
}

int get_child_data(TiXmlNode *p_node, const char *p_data_name, const char *p_attribute, unsigned int *p_data)
{
    int rt = false;
    TiXmlElement *p_element_temp = NULL;
    if (p_node != NULL)
    {
        p_element_temp = p_node->FirstChildElement(p_data_name);
        rt = get_attribute_data(p_element_temp, p_attribute, p_data);
    }

    return rt;
}

// template <typename T>
// int get_child_data(TiXmlNode *p_node, char *p_data_name, char *p_attribute, T *p_data)
// {
//     int rt = false;
//     TiXmlElement *p_element_temp = NULL;
//     p_element_temp = p_node->FirstChildElement(p_data_name);
//     rt = get_attribute_data(p_element_temp, p_attribute, p_data);
//     if(rt == false)
//     {
//         if (strcmp(p_data_name, "fft_start_window") == 0)
//         {
//             *p_data = 0;
//         }
//         else if (strcmp(p_data_name, "fft_finish_window") == 0)
//         {
//             *p_data = 40;
//         }
//     }
//     return rt;
// }
