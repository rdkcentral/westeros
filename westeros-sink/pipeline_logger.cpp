#include <stdio.h>
#include <string>
#include <gst/gst.h>

#define DESC_ARRAY_SIZE 10
static void correct_indentation(std::string *desc, int desc_lines)
{
    int max_size = 0;

    for(int i = 0; i < DESC_ARRAY_SIZE; i++)
    {
        if( (desc_lines & (0x1 << i))  && (desc[i].length() > max_size))
        {
            max_size = desc[i].length();
        }
    }

    for(int i = 0; i < DESC_ARRAY_SIZE; i++)
    {
        if(desc_lines & (0x1 << i))
        {
            int size_diff = max_size - desc[i].length();
            if(size_diff > 0)
            {
                for(int j = 0; j < size_diff; j++)
                {
                    desc[i] = " " + desc[i];
                }
            }
        }
    }
}

static int add_element_to_list(GstElement *element, std::string *desc, std::string *head, int indeces)
{
    GstIterator *it = gst_element_iterate_sink_pads(element);
    bool done = false, has_pad = false, pos_found = false;
    GValue item = G_VALUE_INIT;
    GstPad *pad;
    int size_diff = 0, max_size = 0, index = 0;
    int desc_lines = 0, head_indeces = 0;

    for(int i = 0; i < DESC_ARRAY_SIZE; i++)
    {
        if(indeces & (0x01 << i))
        {
            if( (desc[i].length() > max_size))
            {
                max_size = desc[i].length();                
            }
            if(!pos_found)
            {
                pos_found = true;
                index = i;
            }
            desc_lines = desc_lines | (0x1 << i) ;
        }
    }
    correct_indentation(desc, desc_lines);
    for(int i = 0; i < DESC_ARRAY_SIZE; i++)
    {
        if(indeces & (0x01 << i))
        {
            size_diff = max_size - desc[i].length();
            if(size_diff > 0)
            {
                for(int j = 0; j < size_diff; j++)
                {
                    desc[i] = "─" + desc[i];
                }
            }
            if(i != index)
            {
                auto element_name = gst_element_get_name(element);
                int size = strlen(element_name);
                for (int j = 0; j < size; j++)
                {
                    desc[i] = " " + desc[i];
                }
                g_free(element_name);
            }
            head[i] = "\0";
        }
    }

    auto element_name = gst_element_get_name(element);
    desc[index] = element_name + desc [index];
    g_free(element_name);
    correct_indentation(desc, desc_lines);

    int i = 0;

    while(!done && ( index < DESC_ARRAY_SIZE))
    {
        switch (gst_iterator_next(it, &item)) {
            case GST_ITERATOR_OK:
                pad = static_cast <GstPad *>(g_value_get_object(&item));
                
                if(nullptr == pad)
                {
                    g_value_reset(&item);
                    break;
                }
                
                while(!head[index].empty())
                {
                    index++;
                }
                head[index] = GST_ELEMENT_NAME(element);
                head[index] += ":";
                head[index] += GST_PAD_NAME(pad);
                desc[index] = ">" + desc [index];
                has_pad = true;
                head_indeces |= 0x1 << index;

                g_value_reset(&item);
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(it);
                break;
            default:
                done = TRUE;
                break;
        }
    }
    g_value_unset(&item);
    gst_iterator_free(it);

    if(!has_pad)
    {
        while(!head[index].empty())
        {
            index++;
        }
        head[index] = "--:--";
        head_indeces |= 0x1 << index;
    }

    correct_indentation(desc, head_indeces);
    return desc_lines;
}

static int find_head(GstElement *element, std::string *desc, std::string *head, int index, int &indeces)
{
    GstIterator *it = gst_element_iterate_src_pads(element);
    GValue item = G_VALUE_INIT;
    gboolean done = FALSE;
    GstPad *pad, *peer_pad;
    GstElement *peer_parent;
    std::string src_pad;
    int i, j = 0, pos;

    while (!done) {
        switch (gst_iterator_next(it, &item)) {
            case GST_ITERATOR_OK:
                pad = static_cast <GstPad *>(g_value_get_object(&item));
                if(nullptr == pad)
                {
                    g_value_reset(&item);
                    break;
                }
                peer_pad = gst_pad_get_peer(pad);
                if(peer_pad)
                {
                    peer_parent = gst_pad_get_parent_element(peer_pad);
                    if(peer_parent)
                    {
                        src_pad = GST_ELEMENT_NAME(peer_parent);
                        src_pad += ":";
                        src_pad += GST_PAD_NAME(peer_pad);
                        for(i = 0; i < DESC_ARRAY_SIZE; i++)
                        {
                            if(head[i].compare(src_pad) == 0)
                            {
                                indeces = indeces | (0x1 << i);
                            }
                        }
                        gst_object_unref(peer_parent);
                    }
                    gst_object_unref(peer_pad);
                }
                g_value_reset(&item);
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(it);
                break;
            default:
                done = TRUE;
                break;
        }
    }
    g_value_unset(&item);
    gst_iterator_free(it);

    if(indeces == 0)
    {

        for(i = index; i < DESC_ARRAY_SIZE; i++)
        {
            if(head[i].empty())
            {
                size_t pipeline_start = desc[index].find("]");
                if ((pipeline_start != std::string::npos) && (i != index))
                {
                    int diff = desc[index].size() - pipeline_start;
                    int gap = (diff > 0)? (diff - desc[i].size()) : 0;
                    for(int j = 0; (gap > 0) && (j < gap); j++)
                    {
                        desc [i] = " " + desc[i];
                    }
                    desc [i] = "]" + desc[i];

                }
                indeces = indeces | (0x1 << i);
                break;
            }
        }
    }
    for(int i = 0; i < DESC_ARRAY_SIZE; i++)
    {
        int a = indeces & (0x1 << i);
        if( a != 0)
        {
            pos = i;
            break;
        }
    }
    return pos;
}

static int print_pipeline(GstElement * element, std::string *desc, int index)
{
    GstIterator *itr;
    GstElement * child_element;
    GValue next_item = {0, };
    std::string head[DESC_ARRAY_SIZE] = {};
    int desc_lines = 0, pos = 0;

    desc_lines = 0x1 << index;

    itr = gst_bin_iterate_sorted(GST_BIN(element));

    int numChildren = GST_BIN_NUMCHILDREN(element);
 
    while(numChildren --)
    {
        int indeces = 0;
        switch (gst_iterator_next(itr, &next_item))
        {
            case GST_ITERATOR_OK:
                child_element = GST_ELEMENT(g_value_get_object(&next_item));
                pos = find_head(child_element, desc, head, index, indeces);
                correct_indentation(desc, indeces);
                if(GST_IS_BIN(child_element))
                {
                    for(int i = 0; i < DESC_ARRAY_SIZE; i++)
                    {
                        if(indeces & (0x01 << i))
                        {
                            desc[i] = "]" + desc[i];
                        }
                    }
                    int child_indeces = print_pipeline(child_element, desc, pos);
                    desc_lines = desc_lines | child_indeces;
                    for(int i = 0; i < DESC_ARRAY_SIZE; i++)
                    {
                        if(child_indeces & (0x01 << i))
                        {
                            desc[i] = "[" + desc[i];
                        }
                    }
                }
                desc_lines = desc_lines | add_element_to_list(child_element, desc, head, indeces);
                g_value_reset(&next_item);
                break;
        
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(itr);
                break;

            default:
                break;
        }
    }
    correct_indentation(desc, desc_lines);
    gst_iterator_free(itr);

    return desc_lines;
}

static std::string trim_excess_whitespace(const std::string &str)
{
    std::string result;
    result.reserve(str.size()); // Reserve space to avoid unnecessary reallocations

    bool previous_space = false;
    for(char c : str)
    {
        if (c == ' ')
        {
            if (!previous_space)
            {
                result += c;
            }
            previous_space = true;
        }
        else
        {
            result += c;
            previous_space = false;
        }
    }
    return result;
}

extern "C" void dump_pipeline_info(GstBin *bin)
{
    std::string desc[DESC_ARRAY_SIZE] = {};
    printf("Dumping pipeline: %s\n", GST_ELEMENT_NAME(GST_ELEMENT(bin)));
    for(int i = 0; i < DESC_ARRAY_SIZE; i++)
    {
        desc[i] = "]";
    }
    int indeces = print_pipeline(GST_ELEMENT(bin), desc, 0);

    for(int i = 0; i < DESC_ARRAY_SIZE; i++)
    {
        desc[i] = "[" + desc[i];
    }

    for(int i = 0; i < DESC_ARRAY_SIZE; i++)
    {
        if(indeces & (0x1 << i))
        {
            printf("%d : %s\n", desc[i].size(), trim_excess_whitespace(desc[i]).c_str());
        }
    }
}
