// Doxygen Documentation
/*! \file 	lookUpTable.cpp
 *  \brief  Helperfunctions to Look Up values in LUTs
 *
 * This File contains all the necessary methods to manage LookUpTables
 *
 * \author 		Matthias Werner
 * \date		05/2023
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */


#include "lookUpTable.h"

//*********************************************************************
// Constructor
LookUpTable1D::LookUpTable1D(const uint32_t *axis, const uint32_t *table,
                             uint8_t length, uint8_t fixed_point_decimals)
{
  this->m_x_axis = axis;
  this->m_table = table;
  this->m_length_x_axis = length;
  this->m_fixed_point_decimals = fixed_point_decimals;
}

//*********************************************************************
// Look Up the value in the LookUpTable (fixed point)
bool LookUpTable1D::LookUpValue(uint32_t val, uint32_t *result)
{

  bool look_up_ok = LookUpMap(val);
  *result = this->m_result;

  // return result
  return look_up_ok;
}

//*********************************************************************
// Look Up the value in the LookUpTable (double)
bool LookUpTable1D::LookUpValue(double val, double *result)
{

  // transform fixpoint to double
  double divider = 1;
  uint8_t prec = this->m_fixed_point_decimals;

  while (prec > 0)
  {
    divider = divider * 10;
    prec--;
  }

  val = (val * divider);

  uint32_t x_value = (uint32_t)val;
  bool look_up_ok = LookUpMap(x_value);

  *result = this->m_result / divider;

  // return result
  return look_up_ok;
}

//*********************************************************************
// Find the corresponding position on the Axis
bool LookUpTable1D::LookUpAxisPos(uint32_t x_value)
{
  uint8_t i = 0;

  const uint32_t *previous_axis;
  // Pointer fir axis position to search for
  const uint32_t *pAxis = this->m_x_axis;

  // Check if Minimum has of the axis has been exceeded
  if (x_value <= *pAxis)
  {
    this->m_lower_x_value = *pAxis;
    this->m_upper_x_value = *pAxis;
    this->m_x_position = 0;
    // the value exceeds the min of the axis
    return false;
  }
  else
  {
    // search the axis downwards
    pAxis = pAxis + this->m_length_x_axis - 1;
    for (i = this->m_length_x_axis; i > 0; i--)
    {
      // check if by chance the value matches exactly
      if (x_value == *pAxis)
      {
        this->m_lower_x_value = *pAxis;
        this->m_upper_x_value = *pAxis;
        this->m_x_position = i - 1;
        return true;
      }
      else
      {
        // Found Position on downward search
        if (x_value > *pAxis)
        {
          this->m_lower_x_value = *pAxis;
          // Is it the maximum off the axis values
          if (i == this->m_length_x_axis)
          {
            this->m_upper_x_value = *pAxis;
            this->m_x_position = i - 1;
            // the value exceeds the max of the axis
            return false;
          }
          else
          {
            // else it is a normal midfield position
            this->m_upper_x_value = *previous_axis;
            this->m_x_position = i;
            // value is in between min/max
            return true;
          }
          break;
        }
      }
      previous_axis = pAxis;
      pAxis--;
    }
  }

  return false;
}

//*********************************************************************
// Look Up the value in the LookUp Table
bool LookUpTable1D::LookUpMap(uint32_t x_value)
{
  // Position of the table in memory
  const uint32_t *pTable = this->m_table;

  // Check the position of the x-axis
  bool limits_respected = LookUpAxisPos(x_value);

  uint32_t Y1 = 0;
  uint32_t Y2 = 0;
  int64_t lngTmp;

  // Special cases for the lookup value
  if (this->m_lower_x_value == this->m_upper_x_value)
  {
    if (this->m_x_position == 0)
    {
      // value below minimum
      Y1 = *pTable;
      Y2 = *pTable;
    }
    else if (this->m_x_position == (m_length_x_axis - 1))
    {
      // value above maximum
      pTable = pTable + m_length_x_axis - 1;
      Y1 = *pTable;
      Y2 = *pTable;
    }
    else
    {
      // value matches exactly one position an x-axis
      Y1 = *(pTable + this->m_x_position);
      Y2 = *(pTable + this->m_x_position);
    }
  }
  else
  {
    // Normal case where the value is in between min/max
    Y1 = *(pTable + this->m_x_position - 1);
    Y2 = *(pTable + this->m_x_position);
  }

  // calculate the result
  if (this->m_lower_x_value == this->m_upper_x_value)
  {
    m_result = Y1;
  }
  else
  {
    // linear interpolation
    lngTmp = ((int64_t)Y2 - Y1) * (x_value - this->m_lower_x_value);
    lngTmp = lngTmp / ((int64_t)this->m_upper_x_value - (int64_t)this->m_lower_x_value);
    lngTmp = lngTmp + Y1;
    m_result = (uint32_t)lngTmp;
  }

  // return if the value has been in between min/mx
  return limits_respected;
}