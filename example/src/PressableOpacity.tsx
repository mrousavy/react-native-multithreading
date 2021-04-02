import React, { useCallback } from 'react';
import { PressableProps, Pressable, ViewStyle } from 'react-native';

export interface PressableOpacityProps extends PressableProps {
  disabledOpacity?: number;
  activeOpacity?: number;
}

export const PressableOpacity = ({
  style,
  disabled = false,
  disabledOpacity = 1,
  activeOpacity = 0.2,
  ...passThroughProps
}: PressableOpacityProps): React.ReactElement => {
  const getOpacity = useCallback(
    (pressed: boolean) => {
      if (disabled) {
        return disabledOpacity;
      } else {
        if (pressed) return activeOpacity;
        else return 1;
      }
    },
    [activeOpacity, disabled, disabledOpacity]
  );
  const _style = useCallback(
    ({ pressed }) => [style as ViewStyle, { opacity: getOpacity(pressed) }],
    [getOpacity, style]
  );

  return <Pressable style={_style} disabled={disabled} {...passThroughProps} />;
};
