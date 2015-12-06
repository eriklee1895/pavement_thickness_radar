#ifndef LAYERINFO_H
#define LAYERINFO_H


class LayerInfo
{
public:
    LayerInfo( );

private:
    unsigned char layerIndex;
    unsigned int position;
    float dielectric;
    double amp;
    float velocity;
    float thickness;
    float time_delay;
};

#endif // LAYERINFO_H
