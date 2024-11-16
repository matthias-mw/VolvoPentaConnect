// Doxygen Documentation
/*! \file 	lookUpTable.h
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

#ifndef _lookUpTable_h_
#define _lookUpTable_h_

#include <stdbool.h>
#include <stdint.h>


/*! ************************************************************************
 * \class LookUpTable1D
 * \brief This gives an object for a LookUpTable1D
 * 
 * The LookUp Table consist of an axis and a table with a given size. All
 * data is stored in fixed point notation with a given precision. 
 * 
 */
class LookUpTable1D
{
public:
  /*! ************************************************************************
  * \brief Constructor for the 1D-LookUpTable
  *
  * \param axis     Pointer to the axis (stored const in memory)
  * \param table    Pointer to the table (stored const in memory)
  * \param length   number of point in the axis
  * \param fixed_point_decimals   number of decimals for fixed point notation
  */
  LookUpTable1D(const uint32_t *axis, const uint32_t *table, uint8_t length, 
                uint8_t fixed_point_decimals = 2);

  /*! ************************************************************************
   * \brief Look Up the value in the LookUpTable (fixed point)
   * 
   * This method gives you the corresponding result for the value with is 
   * stored inside a LookUpTable. It also determines if the min/max
   * limits aof the table is respected.
   * 
   * \param val           Value to look up for
   * \param result        result of the lookup in double
   * \return true   value found in between the min/max values of this axis
   * \return false  value exceeds the min/max value
   */
  bool LookUpValue (uint32_t val, uint32_t * result);

  /*! ************************************************************************
   * \brief Look Up the value in the LookUpTable (double)
   * 
   * This method gives you the corresponding result for the value with is 
   * stored inside a LookUpTable. It also determines if the min/max
   * limits aof the table is respected.
   * 
   * \param val           Value to look up for
   * \param result        result of the lookup in double
   * \return true   value found in between the min/max values of this axis
   * \return false  value exceeds the min/max value
   */
  bool LookUpValue (double val, double * result);

private:

  uint8_t m_fixed_point_decimals;
  /** size of the axis*/
  uint8_t m_length_x_axis;
  /** Position of the look up value on the x axis. If the value is in between 
   * two data point it represents the left position */
  uint8_t m_x_position = 0;
  /** corresponding value of the lower (left) x axis datapoint in fixed 
   * point notation*/
  uint32_t m_lower_x_value = 0;
  /** corresponding value of the upper (right) x axis datapoint in fixed 
   * point notation*/
  uint32_t m_upper_x_value = 0;
  /** result for the looked up value in fixed point notation*/  
  uint32_t m_result = 0;

  /** pointer to the x-axis in static memory*/  
  const uint32_t *m_x_axis;
  /** pointer to the map in static memory*/
  const uint32_t *m_table;

  /*! ************************************************************************
   * \brief Find the corresponding position on the Axis
   * 
   * This Method searches for the position of the value inside the axis. The
   * search function performs a downward search from max to min and checks
   * if the min/max limits are respected. 
   * 
   * \note The values of the axis have to be strict monotone rising!
   *
   * \param x_value Value to look up for
   * \return true   value found in between the min/max values of this axis
   * \return false  value exceeds the min/max value
   */
  bool LookUpAxisPos(uint32_t x_value);

  /*! ************************************************************************
   * \brief Look Up the value in the LookUp Table
   * 
   * This method is looking for the matching result for a given value for the 
   * axis of the table. The result is linear interpolated in between two
   * positions of the axis. 
   *
   * \param x_value Value to look up for
   * \return true   value found in between the min/max values of this axis
   * \return false  value exceeds the min/max value
   */
  bool LookUpMap (uint32_t x_value);

};

#endif //_lookUpTable_h_